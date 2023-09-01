
#include "panel.h"
#include "../platform/opengl.h"
#include "../common/common.h"
#include "editor.h"
#include "../common/scanner.h"

#include "panels/scene.h"
#include "panels/asset.h"
#include "panels/timeline.h"
#include "panels/debug.h"

bool Panel::render(Editor* editor, float dt) {
    char nameBuf[256];
    const char* name = this->getName();
    snprintf(nameBuf, sizeof(nameBuf), "%s##%d", name, key);
    bool winOpen = true;
    ImGui::Begin(nameBuf, &winOpen);
    tick(editor, dt);
    ImGui::End();
    return winOpen;
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
        #define X(name, enumName) \
            case PanelType::enumName: { \
                panel = new name ## Panel(); \
                break; \
            }
        PANEL_X
        #undef X 
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
        if(!panels[i]->render(editor, dt)) {
            panels[i]->free();
            delete panels[i];
            panels.removeAt(i);
            i--;
        }
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
                #define X(name, enumName) \
                    if(strcmp(buf, #name) == 0) \
                        type = PanelType::enumName;
                PANEL_X
                #undef X

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

