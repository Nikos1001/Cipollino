
#include "scene.h"
#include "../editor.h"

void ScenePanel::init(int key) {
    Panel::init(key);
    fb.init(100, 100);
    prevMouseInsideViewport = false;
    cam.pos = glm::vec2(0.0f);
    cam.size = 10.0f;
}

void ScenePanel::free() {
    fb.free();
}

void ScenePanel::tick(Editor* editor, float dt) {
    Graphic* g = editor->getOpenGraphic();
    if(g != NULL) {
        int viewW = ImGui::GetContentRegionAvail().x;
        int viewH = ImGui::GetContentRegionAvail().y;
        glm::vec2 winPos = glm::vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

        glm::vec2 winMousePos = (editor->app->getMousePos() - winPos);
        float aspect = (float)viewW / (float)viewH;
        glm::vec4 normMousePos = glm::vec4(2 * winMousePos.x / (float)viewW - 1.0f, -2 * winMousePos.y / (float)viewH + 1.0f, 0.0f, 1.0f);
        glm::mat4 invProjView = glm::inverse(cam.projView(aspect));
        glm::vec2 mousePos = invProjView * normMousePos; 

        bool mouseInsideViewport = false;
        if(ImGui::IsWindowFocused()) {
            mouseInsideViewport = winMousePos.x >= 0 && winMousePos.x < viewW && winMousePos.y >= 0 && winMousePos.y < viewH;
            if(mouseInsideViewport) {
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    editor->currTool->mouseClick(editor, mousePos);
                }
                if(ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    if(prevMouseInsideViewport) {
                        editor->currTool->mouseDown(editor, mousePos);
                    } else {
                        editor->currTool->mouseClick(editor, mousePos);
                    }
                }
                if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    editor->currTool->mouseRelease(editor, mousePos);
                }
            }
        } 
        if(prevMouseInsideViewport && !mouseInsideViewport) 
            editor->currTool->mouseRelease(editor, mousePos);
        prevMouseInsideViewport = mouseInsideViewport;

        SceneRenderParams params;
        params.proj = &editor->proj;
        params.graphicKey = editor->openGraphic;
        params.w = 2 * viewW; // double the resolution for AA
        params.h = 2 * viewH;
        params.fb = &fb;
        params.cam = &cam;
        params.frame = editor->getFrame();
        params.onionBefore = editor->onionSkinBefore;
        params.onionAfter = editor->onionSkinAfter;
        if(editor->playing) {
            params.onionBefore = 0;
            params.onionAfter = 0;
        }
        editor->sceneRndr.render(params);
        Framebuffer::renderToScreen(editor->app->getW(), editor->app->getH());

        ImGui::Image((void*)(intptr_t)fb.color, ImVec2(viewW, viewH), ImVec2(0, 1), ImVec2(1, 0));

        if(ImGui::IsWindowHovered()) {
            float wheel = ImGui::GetIO().MouseWheel;
            float zoomFac = powf(1.05f, -wheel);
            cam.pos -= (mousePos - cam.pos) * (zoomFac - 1);
            cam.size *= zoomFac;
        }

    }
}

const char* ScenePanel::getName() {
    return "Scene";
}

size_t ScenePanel::getSize() {
    return sizeof(ScenePanel);
}

