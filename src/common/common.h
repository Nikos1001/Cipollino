
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void errPrint(const char* fmt, ...);
#define ASSERT(cond, msg) if(!(cond)) {errPrint(msg "\n"); exit(-1);}

#endif
