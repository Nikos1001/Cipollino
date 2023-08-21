
#ifndef SHADER_H
#define SHADER_H

class Shader {
public:
    void init(const char* vertSrc, const char* fragSrc);
    void use();
private:
    unsigned int program;
};

#endif
