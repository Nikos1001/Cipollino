
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

        uniform mediump vec4 uColor;

        void main() {
            
            gl_FragColor = uColor;

        }
    )");

}

void SceneRenderer::free() {
    strokeShader.free();
}

void SceneRenderer::renderFrame(Project* proj, Frame* f, bool onionSkin) {
    for(int i = 0; i < f->strokes.cnt(); i++) {
        Stroke* s = proj->getStroke(f->strokes[i]);
        if(!onionSkin)
            strokeShader.setVec4("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        if(s != NULL)
            s->mesh.render();
    }
}

void SceneRenderer::render(SceneRenderParams params) {
    params.fb->resize(params.w, params.h);
    params.fb->renderTo();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float aspect = (float)params.w / (float)params.h;
    strokeShader.use();
    glm::mat4 trans = params.cam->projView(aspect); 
    strokeShader.setMat4("uTrans", trans);
    
    Graphic* g = params.proj->getGraphic(params.graphicKey);
    if(g != NULL) {
        for(int i = 0; i < g->layers.cnt(); i++) {
            Layer* l = params.proj->getLayer(g->layers[i]);
            Frame* f = l->getFrameAt(params.proj, params.frame);
            if(f != NULL) {
                float alpha = 0.5f;
                Frame* curr = f;
                for(int j = 0; j < params.onionBefore; j++) {
                    strokeShader.setVec4("uColor", glm::vec4(1.0f, 0.0f, 0.8f, alpha));
                    alpha = 0.5 * alpha + 0.025;
                    curr = l->getFrameAt(params.proj, curr->begin - 1);
                    if(curr == NULL)
                        break;
                    renderFrame(params.proj, curr, true);
                }
                alpha = 0.5f;
                curr = f;
                for(int j = 0; j < params.onionAfter; j++) {
                    strokeShader.setVec4("uColor", glm::vec4(0.0f, 0.8f, 1.0f, alpha));
                    alpha = 0.5 * alpha + 0.025;
                    curr = l->getFrameAfter(params.proj, curr->begin);
                    if(curr == NULL)
                        break;
                    renderFrame(params.proj, curr, true);        
                }
            }
        }


        for(int i = 0; i < g->layers.cnt(); i++) {
            Layer* l = params.proj->getLayer(g->layers[i]);
            Frame* f = l->getFrameAt(params.proj, params.frame);
            if(f != NULL) {
                renderFrame(params.proj, f, false);        
            }
        }
    }
}
