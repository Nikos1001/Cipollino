
#include "common.h"

void errPrint(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
#ifndef __EMSCRIPTEN__
    vfprintf(stderr, fmt, args);
#else
    vprintf(fmt, args);
#endif
    va_end(args);
}
