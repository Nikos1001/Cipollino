
#ifndef SHADER_H
#define SHADER_H

#include "../common/common.h"

class Shader {
public:
    void init(const char* vertSrc, const char* fragSrc);
    void free();
    void use();

    void setMat4(const char* name, glm::mat4 val); 
private:
    unsigned int program;
};

#endif
