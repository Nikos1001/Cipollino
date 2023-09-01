
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

    void render(Project* proj, Key graphicKey, int w, int h, Framebuffer* fb, Camera* cam, int frame);
private:
    Shader strokeShader;
};

#endif
