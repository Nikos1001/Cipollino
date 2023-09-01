
#include "editor.h"
#include "../protocol/protocol.h"

void Editor::init(Socket* sock, App* app) {
    this->sock = sock;
    this->app = app;
    keys.init();
    proj.init();

    sentGet = false;

    sceneRndr.init();

    panels.init();
    panels.loadSettings(this);

    acts.init();

    pencil.init();
    currTool = &pencil;
    openGraphic = NULL_KEY;
    activeLayer = 0;

    playing = false;

}

void Editor::tick(float dt) {

    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("Edit")) {
            if(ImGui::MenuItem("Undo", "Ctrl Z", false, acts.hasUndo()))
                acts.undo(); 
            if(ImGui::MenuItem("Redo", "Ctrl Y", false, acts.hasRedo()))
                acts.redo();
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View")) {
            if(ImGui::BeginMenu("Add panel")) {
                #define X(name, enumName) \
                    if(ImGui::MenuItem(#name)) \
                        panels.addPanel(PanelType::enumName);
                PANEL_X
                #undef X
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    Graphic* g = getOpenGraphic();
    if(g != NULL) {
        if(activeLayer < 0)
            activeLayer = 0;
        if(activeLayer >= g->layers.cnt())
            activeLayer = g->layers.cnt() - 1;
    }

    if(!ImGui::GetIO().WantTextInput && ImGui::GetIO().KeyMods == ImGuiMod_Super) {
        if(ImGui::IsKeyPressed(ImGuiKey_Z, false))
            acts.undo();
        if(ImGui::IsKeyPressed(ImGuiKey_Y, false))
            acts.redo();
    }
    if(!ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
        playing = !playing;
    }

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    if(keys.outOfKeys()) {
        ImGui::OpenPopup("Out of keys");
    }
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Out of keys", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text("Out of keys :(");

        if(!keys.outOfKeys())
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if(sock->ready() && !sentGet) {
        uint8_t msg = MessageType::GET;
        sock->send(&msg, sizeof(msg));
        sentGet = true;
    }

    if(playing)
        time += dt;

    panels.tick(this, dt);
    panels.saveSettings(this);

    keys.sendRequests(sock);

}

void Editor::handleMsg(SocketMsg* msg, uint8_t msgType) {
    if(msgType == MessageType::DATA) {
        proj.loadFrom(msg);
    }
    if(msgType == MessageType::UPDATE) {
        proj.applyUpdate(msg);
    }
    if(msgType == MessageType::ADD_UPDATE) {
        Key key = msg->readKey();
        proj.applyAddUpdate(msg, key);
    }
    if(msgType == MessageType::KEY_GRANT) {
        keys.receiveKeys(msg);
    }
}

void Editor::free() {
    proj.free();
    acts.free();
    sceneRndr.free();
    panels.free();
}

Graphic* Editor::getOpenGraphic() {
    return proj.getGraphic(openGraphic);
}

Layer* Editor::getActiveLayer() {
    Graphic* g = getOpenGraphic();
    if(g == NULL)
        return NULL;
    return proj.getLayer(g->layers[activeLayer]);
}

int Editor::getFrame() {
    return (int)(time / (1.0f / proj.fps));
}
