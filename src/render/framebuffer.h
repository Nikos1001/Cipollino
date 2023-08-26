
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../platform/opengl.h"

class Framebuffer {
public:
    void init(int w, int h);
    void free();
    void renderTo();
    void resize(int w, int h);
    
    static void renderToScreen(int w, int h) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, w, h);
    }

    static void renderTo(Framebuffer* fb, int screenW, int screenH) {
        if(fb == NULL) {
            renderToScreen(screenW, screenH);
        } else {
            fb->renderTo();
        }
    }

    unsigned int color;
private:
    unsigned int fbo;
    int w, h;
};


#endif
