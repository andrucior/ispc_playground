#ifdef _WIN32
#include <windows.h>
#define rdtsc __rdtsc

static LARGE_INTEGER freq;
static LARGE_INTEGER t_start;

static inline void reset_and_start_timer() {
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t_start);
}

static inline double get_elapsed_msec() {
    LARGE_INTEGER t_end;
    QueryPerformanceCounter(&t_end);
    return (double)(t_end.QuadPart - t_start.QuadPart) * 1000.0 / freq.QuadPart;
}

#else // Linux/macOS/ARM
#include <sys/time.h>
#include <stdint.h>
static uint64_t rdtsc() { ... } // inline assembly or ARM version
static inline double rtc(void) { ... }
#endif
