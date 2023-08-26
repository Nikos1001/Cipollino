
#include "framebuffer.h"
#include "../common/common.h"

void Framebuffer::init(int w, int h) {
    this->w = w;
    this->h = h;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &color);
    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

    ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer incomplete.")
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::free() {
    glDeleteTextures(1, &color);
    glDeleteFramebuffers(1, &fbo);
}

void Framebuffer::renderTo() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, w, h);
}

void Framebuffer::resize(int w, int h) {
    if(this->w == w && this->h == h)
        return;
    if(w <= 0 || h <= 0)
        return;
    free();
    init(w, h);
}
