
#ifndef SCENE_H
#define SCENE_H

#include "shader.h"
#include "framebuffer.h"
#include "../project/project.h"
#include "camera.h"

struct SceneRenderParams {
    Project* proj;
    Key graphicKey;
    int w, h;
    Framebuffer* fb;
    Camera* cam;
    int frame;

    int onionBefore;
    int onionAfter;
};

class SceneRenderer {
public:
    void init();
    void free();

    void render(SceneRenderParams params);
private:
    Shader strokeShader;

    void renderFrame(Project* proj, Frame* f, bool onionSkin);
};

#endif
