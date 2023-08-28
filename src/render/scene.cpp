
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

void SceneRenderer::render(Project* proj, Key graphicKey, int w, int h, Framebuffer* fb, Camera* cam) {
    fb->resize(w, h);
    fb->renderTo();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    float aspect = (float)w / (float)h;
    strokeShader.use();
    glm::mat4 trans = cam->projView(aspect); 
    strokeShader.setMat4("uTrans", trans);
    
    Graphic* g = proj->getGraphic(graphicKey);
    if(g != NULL) {
        for(int i = 0; i < g->strokes.cnt(); i++) {
            Stroke* s = &g->strokes[i];
            s->mesh.render();
        }
    }
}
