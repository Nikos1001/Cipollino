
#include "panel.h"
#include "../platform/opengl.h"
#include "../common/common.h"
#include "editor.h"
#include "../common/scanner.h"

void Panel::render(Editor* editor, float dt) {
    char nameBuf[256];
    const char* name = this->getName();
    snprintf(nameBuf, sizeof(nameBuf), "%s##%d", name, key);
    ImGui::Begin(nameBuf);
    tick(editor, dt);
    ImGui::End();
}



void PanelManager::init() {
    panels.init();
    currKey = 0;
}

void PanelManager::free() {
    for(int i = 0; i < panels.cnt(); i++) {
        panels[i]->free();
        delete panels[i];
    }
    panels.free();
}

bool PanelManager::addPanelWithKey(PanelType type, int key) {
    Panel* panel = NULL;
    switch(type) {
        case SCENE: {
            panel = new ScenePanel();
            break;
        }
        case DEBUG: {
            panel = new DebugPanel();
            break;
        }
        default:
            return false;
    }
    panel->init(key);
    panels.add(panel);
    return true;
}

void PanelManager::addPanel(PanelType type) {
    if(addPanelWithKey(type, currKey)) {
        currKey++;
    }
}

void PanelManager::tick(Editor* editor, float dt) {
    for(int i = 0; i < panels.cnt(); i++) {
        panels[i]->render(editor, dt);
    }
}

void PanelManager::loadSettings(Editor* editor) {
    const char* imguiIni = editor->app->loadSetting("imguiIni");
    if(imguiIni != NULL) {
        ImGui::LoadIniSettingsFromMemory(imguiIni);
        anim::strfree(imguiIni);
    }
    const char* panels = editor->app->loadSetting("panels");
    if(panels != NULL) {
        Scanner scnr;
        scnr.init(panels);
        
        do {
            if(!scnr.readInt(&currKey))
                break;
            int cnt;
            if(!scnr.readInt(&cnt))
                break;
            for(int i = 0; i < cnt; i++) {
                char buf[64];
                if(!scnr.readStr(buf, 64))
                    break;
                int key;
                if(!scnr.readInt(&key))
                    break;

                PanelType type = PanelType::NONE;
                if(strcmp(buf, "Scene") == 0)
                    type = PanelType::SCENE;
                if(strcmp(buf, "Debug") == 0)
                    type = PanelType::DEBUG;

                if(type != PanelType::NONE)
                    addPanelWithKey(type, key);
            }
        } while(0);

        anim::strfree(panels);
    }
}

void PanelManager::saveSettings(Editor* editor) {
    if(ImGui::GetIO().WantSaveIniSettings) {
        editor->app->saveSetting("imguiIni", ImGui::SaveIniSettingsToMemory());

        char panelDataBuf[64 + 64 * panels.cnt()];
        size_t sizeLeft = sizeof(panelDataBuf);
        char* c = panelDataBuf;

        size_t sizeTaken = snprintf(c, sizeLeft, "%d ", currKey);
        c += sizeTaken;
        sizeLeft -= sizeTaken;

        sizeTaken = snprintf(c, sizeLeft, "%d ", panels.cnt());
        c += sizeTaken;
        sizeLeft -= sizeTaken;

        for(int i = 0; i < panels.cnt(); i++) {
            sizeTaken = snprintf(c, sizeLeft, "%s %d ", panels[i]->getName(), panels[i]->key);
            sizeLeft -= sizeTaken;
            c += sizeTaken;
        }
        editor->app->saveSetting("panels", panelDataBuf);
    }
}


void ScenePanel::init(int key) {
    Panel::init(key);
    fb.init(100, 100);
    prevMouseInsideViewport = false;
}

void ScenePanel::free() {
    fb.free();
}

void ScenePanel::tick(Editor* editor, float dt) {
    int viewW = ImGui::GetContentRegionAvail().x;
    int viewH = ImGui::GetContentRegionAvail().y;
    glm::vec2 winPos = glm::vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

    glm::vec2 winMousePos = (editor->app->getMousePos() - winPos);
    float aspect = (float)viewW / (float)viewH;
    glm::vec2 mousePos = winMousePos / ((float)viewH / 10.0f) - glm::vec2(aspect * 5.0f, 5.0f);
    mousePos.y *= -1;

    bool mouseInsideViewport = false;
    if(ImGui::IsWindowFocused()) {
        mouseInsideViewport = winMousePos.x >= 0 && winMousePos.x < viewW && winMousePos.y >= 0 && winMousePos.y < viewH;
        if(mouseInsideViewport) {
            if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) || !prevMouseInsideViewport) {
                editor->currTool->mouseClick(editor, mousePos);
            }
            if(ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                editor->currTool->mouseDown(editor, mousePos);
            }
            if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                editor->currTool->mouseRelease(editor, mousePos);
            }
        }
    } 
    if(prevMouseInsideViewport && !mouseInsideViewport) 
        editor->currTool->mouseRelease(editor, mousePos);
    prevMouseInsideViewport = mouseInsideViewport;

    // double the resolution for AA
    editor->sceneRndr.render(2 * viewW, 2 * viewH, &editor->proj, &fb);
    Framebuffer::renderToScreen(editor->app->getW(), editor->app->getH());

    // an image button is needed to make sure the window doensn't get dragged
    ImGui::ImageButton((void*)(intptr_t)fb.color, ImVec2(viewW, viewH), ImVec2(0, 1), ImVec2(1, 0), 0);

}

const char* ScenePanel::getName() {
    return "Scene";
}

size_t ScenePanel::getSize() {
    return sizeof(ScenePanel);
}



void DebugPanel::init(int key) {
    Panel::init(key);
}

void DebugPanel::free() {

}

void DebugPanel::tick(Editor* editor, float dt) {
    ImGui::Text("dt: %g", dt);
    ImGui::Text("mem: %zu", anim::memUsed);
}

const char* DebugPanel::getName() {
    return "Debug";
}

size_t DebugPanel::getSize() {
    return sizeof(DebugPanel);
}
