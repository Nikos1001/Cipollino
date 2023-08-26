
#include "editor.h"
#include "../protocol/protocol.h"

void Editor::init(Socket* sock, App* app) {
    this->sock = sock;
    this->app = app;
    keys.init();
    proj.init();

    acts.init();

    sentGet = false;

    sceneRndr.init();

    pencil.init();
    currTool = &pencil;

    panels.init();
    panels.loadSettings(this);

}

void Editor::tick(float dt) {

    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("Edit")) {
            if(ImGui::MenuItem("Undo", "Ctrl Z", false, acts.hasUndo()))
                acts.undo(this);
            if(ImGui::MenuItem("Redo", "Ctrl Y", false, acts.hasRedo()))
                acts.redo(this);
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View")) {
            if(ImGui::BeginMenu("Add panel")) {
                if(ImGui::MenuItem("Scene"))
                    panels.addPanel(SCENE);
                if(ImGui::MenuItem("Debug"))
                    panels.addPanel(DEBUG);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if(ImGui::GetIO().KeyMods == ImGuiMod_Super) {
        if(ImGui::IsKeyPressed(ImGuiKey_Z))
            acts.undo(this);
        if(ImGui::IsKeyPressed(ImGuiKey_Y))
            acts.redo(this);
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
