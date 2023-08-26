
#include "shader.h"
#include "../platform/opengl.h"
#include "../common/common.h"

unsigned int makeShader(const char* src, bool frag) {
    unsigned int shader = glCreateShader(frag ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        errPrint("Shader compilation error:\n%s", log);
        ASSERT(false, "Fix the shader pls");
    }
    return shader;
}

void Shader::init(const char* vertSrc, const char* fragSrc) {
    program = glCreateProgram();
    unsigned int vertShader = makeShader(vertSrc, false);
    unsigned int fragShader = makeShader(fragSrc, true);
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        errPrint("Shader linking error:\n%s", log);
        ASSERT(false, "Fix the shader pls");
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void Shader::free() {
    glDeleteProgram(program);
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::setMat4(const char* name, glm::mat4 val) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, false, glm::value_ptr(val));
}
