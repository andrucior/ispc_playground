#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION

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
#include "common/timing.h"
#include "common/stb_image_write.h"

using namespace ispc;

struct RGB { int r, g, b; };

// Based on Intel Corporation examples
// source: https://github.com/ispc/ispc/releases/tag/v1.28.2
 
static void writePNG(int* buf, int width, int height, const char* fn) {
    // Function taking a buffer buf and saving it to a file fn
    std::vector<unsigned char> rgb(width * height * 3);
    
    for (int i = 0; i < width * height; ++i) 
    {
        char c = (buf[i] & 0x1) ? (char)240 : 20;
        rgb[i * 3 + 0] = c; // R
        rgb[i * 3 + 1] = c; // G
        rgb[i * 3 + 2] = c; // B
    }

    if (stbi_write_png(fn, width, height, 3, rgb.data(), width * 3) == 0) 
    {
        fprintf(stderr, "Failed to write PNG file %s\n", fn);
        exit(1);
    }
    printf("Wrote image file %s\n", fn);
}

int main(int argc, char* argv[]) 
{
    static unsigned int test_iterations[] = { 3, 3 };
    unsigned int width = 768;
    unsigned int height = 512;
    float x0 = -2;
    float x1 = 1;
    float y0 = -1;
    float y1 = 1;

    if (argc > 1) 
    {
        if (strncmp(argv[1], "--scale=", 8) == 0) {
            float scale = atof(argv[1] + 8);
            width *= scale;
            height *= scale;
        }
    }

    if ((argc == 3) || (argc == 4))
    {
        for (int i = 0; i < 2; i++)
        {
            test_iterations[i] = atoi(argv[argc - 2 + i]);
        }
    }

    int maxIterations = 256;
    int* buf = new int[width * height];

    // Compute the image using the ispc implementation; report the minimum
    // time of three runs.
    double minISPC = 1e30;
    for (unsigned int i = 0; i < test_iterations[0]; ++i) {
        reset_and_start_timer();
        mandelbrot_ispc(x0, y0, x1, y1, width, height, maxIterations, buf);
        double dt = get_elapsed_mcycles();
        printf("@time of ISPC run:\t\t\t[%.3f] million cycles\n", dt);
        minISPC = std::min<double>(minISPC, dt);
    }

    printf("@time of ISPC run : \t\t\t[% .3f] million cycles\n", minISPC);

    const char* filename = "mandelbrot-ispc.png";
    writePNG(buf, width, height, filename);
    
    return 0;
}
