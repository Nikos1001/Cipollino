
#include "scanner.h"

void Scanner::init(const char* s) {
    this->s = s;
}

bool Scanner::readInt(int* out) {
    *out = 0;
    while(*s != '\0' && *s != ' ') {
        char c = *s;
        s++;
        if(c < '0' || c > '9')
            return false;
        *out *= 10;
        *out += c - '0';
    }
    if(*s == ' ')
        s++;
    return true;
}

bool Scanner::readStr(char* buf, int bufSize) {
    int i;
    for(i = 0; i < bufSize - 1; i++) {
        char c = *s;
        if(c == '\0' || c == ' ')
            break;
        s++;
        buf[i] = c;
    }
    buf[i] = '\0';
    if(*s == ' ')
        s++;
    return true;
}
