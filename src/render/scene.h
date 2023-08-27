
#ifndef SCENE_H
#define SCENE_H

#include "shader.h"
#include "framebuffer.h"
#include "../project/project.h"
#include "camera.h"

class SceneRenderer {
public:
    void init();
    void free();

    void render(int w, int h, Project* proj, Framebuffer* fb, Camera* cam);
private:
    Shader strokeShader;
};

#endif
