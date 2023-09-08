
#include "editor.h"
#include "../protocol/protocol.h"
#include "gui.h"
#include "panels/timeline.h"

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

    strokeCol = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    playing = false;
    time = 0.0f;

    onionSkinBefore = 1;
    onionSkinAfter = 1;

}

void Editor::tick(float dt) {

    // ImGui::ShowDemoWindow();

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
    Layer* l = NULL;
    if(g != NULL) {
        if(activeLayer < 0)
            activeLayer = 0;
        if(activeLayer >= g->layers.cnt())
            activeLayer = g->layers.cnt() - 1;
        if(g->layers.cnt() > 0)
            l = proj.getLayer(g->layers[activeLayer]);
    }

    if(canDoShortcuts()) {
        if(ImGui::GetIO().KeyMods == ImGuiMod_Super) {
            if(ImGui::IsKeyPressed(ImGuiKey_Z))
                acts.undo();
            if(ImGui::IsKeyPressed(ImGuiKey_Y))
                acts.redo();
        }
        if(ImGui::IsKeyPressed(ImGuiKey_Space))
            playing = !playing;
        if(ImGui::IsKeyPressed(ImGuiKey_Period)) {
            if(shiftDown()) {
                goToNextFrame(this);
            } else {
                time += proj.frameLen();
            }
        }
        if(ImGui::IsKeyPressed(ImGuiKey_Comma)) {
            if(shiftDown()) {
                goToPrevFrame(this);
            } else {
                time -= proj.frameLen();
            }
        }
        if(ImGui::IsKeyPressed(ImGuiKey_K, false)) {
            if(l != NULL) {
                Frame* f = l->getFrameStartingAt(&proj, getFrame());
                if(f == NULL) {
                    EditorAction createFrame;
                    createFrame.init(this);
                    proj.addFrame(keys.nextKey(), l->key, getFrame(), &createFrame);
                    acts.pushAction(createFrame);
                }
            }
        }
    }

    if(g != NULL) {
        if(playing)
            time += dt;
        time = fmax(0.0f, time);
        if(time > g->len * proj.frameLen())
            time = 0.0f; 
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
    return (int)(time / proj.frameLen());
}

void Editor::setTime(float t) {
    time = t;
    playing = false;
}
