
#include "scene.h"

void SceneRenderer::init() {

    strokeShader.init(R"(
        #version 100 

        attribute vec2 aPos;

        uniform mat4 uTrans;

        void main() {
            gl_Position = uTrans * vec4(aPos, 0.0, 1.0);                
        }

    )", R"(
        #version 100 

        void main() {
            
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

        }
    )");

}

void SceneRenderer::free() {
    strokeShader.free();
}

void SceneRenderer::render(int w, int h, Project* proj, Framebuffer* fb) {
    fb->resize(w, h);
    fb->renderTo();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    float aspect = (float)w / (float)h;
    strokeShader.use();
    glm::mat4 trans = glm::ortho(-5.0f * aspect, 5.0f * aspect, -5.0f, 5.0f, -1.0f, 1.0f);
    strokeShader.setMat4("uTrans", trans);
    for(int i = 0; i < proj->strokes.cnt(); i++) {
        Stroke* s = &proj->strokes[i];
        s->mesh.render();
    }
}
