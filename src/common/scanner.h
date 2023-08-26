
#ifndef SCANNER_H
#define SCANNER_H

class Scanner {
public:
    void init(const char* s);
    bool readInt(int* out);
    bool readStr(char* buf, int bufSize);
private:
    const char* s;
};

#endif
