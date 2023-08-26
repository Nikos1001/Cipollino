
#ifndef SCENE_H
#define SCENE_H

#include "shader.h"
#include "framebuffer.h"
#include "../project/project.h"

class SceneRenderer {
public:
    void init();
    void free();

    void render(int w, int h, Project* proj, Framebuffer* fb);
private:
    Shader strokeShader;
};

#endif
