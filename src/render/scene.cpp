
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

enum FrameRenderMode {
    NORMAL,
    ONION_SKIN,
    PICKING 
};

void SceneRenderer::renderFrame(Project* proj, Frame* f, FrameRenderMode renderMode) {
    for(int i = 0; i < f->strokes.cnt(); i++) {
        Stroke* s = proj->getStroke(f->strokes[i]);
        switch(renderMode) {
            case NORMAL: {
                strokeShader.setVec4("uColor", s->color);
                break;
            }
            case ONION_SKIN:
                break;
            case PICKING: {
                int r = (i + 1) & 0xFF;
                int g = ((i + 1) >> 8) & 0xFF;
                int b = ((i + 1) >> 16) & 0xFF;
                int a = ((i + 1) >> 24) & 0xFF;
                glm::vec4 color = glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
                strokeShader.setVec4("uColor", color);
            }
        }
        s->mesh.render();
    }
}

void SceneRenderer::render(SceneRenderParams params) {
        float aspect = (float)params.w / (float)params.h;
    strokeShader.use();
    glm::mat4 trans = params.cam->projView(aspect); 
    strokeShader.setMat4("uTrans", trans);
    
    Graphic* g = params.proj->getGraphic(params.graphicKey);
    if(g != NULL) {
        params.fb->resize(params.w, params.h);
        params.fb->renderTo();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for(int i = 0; i < g->layers.cnt(); i++) {
            Layer* l = params.proj->getLayer(g->layers[i]);
            Frame* f = l->getFrameAt(params.proj, params.frame);
            float alpha = 0.5f;
            Frame* curr = l->getFrameAt(params.proj, f == NULL ? params.frame - 1 : f->begin - 1);
            for(int j = 0; j < params.onionBefore; j++) {
                if(curr == NULL)
                    break;
                strokeShader.setVec4("uColor", glm::vec4(1.0f, 0.0f, 0.8f, alpha));
                alpha = 0.5 * alpha + 0.025;
                renderFrame(params.proj, curr, true);
                curr = l->getFrameAt(params.proj, curr->begin - 1);
            }
            alpha = 0.5f;
            curr = l->getFrameAfter(params.proj, params.frame);
            for(int j = 0; j < params.onionAfter; j++) {
                if(curr == NULL)
                    break;
                strokeShader.setVec4("uColor", glm::vec4(0.0f, 0.8f, 1.0f, alpha));
                alpha = 0.5 * alpha + 0.025;
                renderFrame(params.proj, curr, NORMAL);        
                curr = l->getFrameAfter(params.proj, curr->begin);
            }
        }


        for(int i = 0; i < g->layers.cnt(); i++) {
            Layer* l = params.proj->getLayer(g->layers[i]);
            Frame* f = l->getFrameAt(params.proj, params.frame);
            if(f != NULL) {
                renderFrame(params.proj, f, ONION_SKIN);        
            }
        }

        if(params.picking != NULL) {
            params.picking->resize(params.w, params.h);
            params.picking->renderTo();
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDisable(GL_BLEND);
            for(int i = 0; i < g->layers.cnt(); i++) {
                Layer* l = params.proj->getLayer(g->layers[i]);
                Frame* f = l->getFrameAt(params.proj, params.frame);
                if(f != NULL) {
                    renderFrame(params.proj, f, PICKING);
                }
            }
        }
    }
}
