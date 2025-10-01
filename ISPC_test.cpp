// ISPC_test.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include "hello.h"
#include "mandelbrot.h"
#include "./common/timing.h"

using namespace ispc;

struct RGB { int r, g, b; };

static void writePPM(int* buf, int width, int height, const char* fn) {
    FILE* fp = fopen(fn, "wb");
    if (!fp) {
        printf("Couldn't open a file '%s'\n", fn);
        exit(-1);
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int i = 0; i < width * height; ++i) {
        // Map the iteration count to colors by just alternating between
        // two greys.
        char c = (buf[i] & 0x1) ? (char)240 : 20;
        for (int j = 0; j < 3; ++j)
            fputc(c, fp);
    }
    fclose(fp);
    printf("Wrote image file %s\n", fn);
}

static void printPPMConsole(const std::vector<RGB>& pixels, int width, int height) {
    for (int y = 0; y < height; y += 4) { // skip some rows to fit console
        for (int x = 0; x < width; x += 2) { // skip some columns
            const RGB& p = pixels[y * width + x];
            // Convert to grayscale
            int gray = (p.r + p.g + p.b) / 3;
            char c;
            if (gray > 200) c = ' ';
            else if (gray > 150) c = '.';
            else if (gray > 100) c = '*';
            else if (gray > 50)  c = 'O';
            else                c = '@';
            std::cout << c;
        }
        std::cout << "\n";
    }
}

// Read P6 binary PPM
static void ReadPPMFile(std::ifstream& file, int& width, int& height, std::vector<RGB>& pixels) {
    std::string type;
    int maxval;

    file >> type;
    if (type != "P6") {
        std::cerr << "Only P6 binary PPM supported\n";
        exit(1);
    }

    file >> width >> height >> maxval;
    file.get(); // skip single whitespace after header

    pixels.resize(width * height);
    file.read(reinterpret_cast<char*>(pixels.data()), width * height * 3);

    std::cout << "Read " << width << "x" << height << " PPM image\n";
    printPPMConsole(pixels, width, height);
}

int main(int argc, char* argv[]) {
    static unsigned int test_iterations[] = { 3, 3 };
    unsigned int width = 768;
    unsigned int height = 512;
    float x0 = -2;
    float x1 = 1;
    float y0 = -1;
    float y1 = 1;

    if (argc > 1) {
        if (strncmp(argv[1], "--scale=", 8) == 0) {
            float scale = atof(argv[1] + 8);
            width *= scale;
            height *= scale;
        }
    }
    if ((argc == 3) || (argc == 4)) {
        for (int i = 0; i < 2; i++) {
            test_iterations[i] = atoi(argv[argc - 2 + i]);
        }
    }

    int maxIterations = 256;
    int* buf = new int[width * height];

    //
    // Compute the image using the ispc implementation; report the minimum
    // time of three runs.
    //
    double minISPC = 1e30;
    for (unsigned int i = 0; i < test_iterations[0]; ++i) {
        reset_and_start_timer();
        mandelbrot_ispc(x0, y0, x1, y1, width, height, maxIterations, buf);
    }

    std::string filename = "mandelbrot-ispc.ppm";
    writePPM(buf, width, height, filename.c_str());
    std::ifstream file(filename);
    
    std::vector<RGB> pixels;
    ReadPPMFile(file, reinterpret_cast<int&>(width), reinterpret_cast<int&>(height), pixels);
    file.close();
    return 0;
}
