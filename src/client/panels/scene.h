
#ifndef SCENE_PANEL_H
#define SCENE_PANEL_H

#include "../panel.h"

class ScenePanel : public Panel {
public:
    void init(int key);
    void free();
    void tick(Editor* editor, float dt);
    const char* getName();
    size_t getSize();
private:
    Framebuffer fb;
    bool prevMouseInsideViewport;
    Camera cam;
};

#endif
