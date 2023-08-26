
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../../libs/glm/glm/glm.hpp"
#include "../../libs/glm/glm/gtc/matrix_transform.hpp"
#include "../../libs/glm/glm/gtc/type_ptr.hpp"

namespace anim {
    extern size_t memUsed;
    void* malloc(size_t size);
    void* realloc(void* ptr, size_t oldSize, size_t newSize);
    void free(void* ptr, size_t size);
    void strfree(const char* str);
}

void errPrint(const char* fmt, ...);
#define ASSERT(cond, msg) if(!(cond)) {errPrint(msg "\n"); exit(-1);}

#endif
