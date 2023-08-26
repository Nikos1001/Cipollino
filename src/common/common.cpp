
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

#include <cstdlib>

size_t anim::memUsed = 0;

void* anim::malloc(size_t size) {
    memUsed += size;
    return std::malloc(size);
}

void* anim::realloc(void* ptr, size_t oldSize, size_t newSize) {
    memUsed += newSize - oldSize;
    return std::realloc(ptr, newSize);
}

void anim::free(void* ptr, size_t size) {
    memUsed -= size;
    std::free(ptr);
}

void anim::strfree(const char* str) {
    anim::free((void*)str, strlen(str) + 1);
}
