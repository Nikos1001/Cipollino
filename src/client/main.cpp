
#include <stdio.h>
#include "../platform/platform.h"

#include "../protocol/protocol.h"
#include "editor.h"

class Client : public App {
    void init() {
        runEditor = false;
        host[0] = '\0';
        
        const char* savedHost = loadSetting("host");
        if(savedHost != NULL) {
            strncpy(host, savedHost, 256);
        }

        initedSock = false;
    }

    void tick(float dt) {

        if(runEditor) {
            editor.tick(dt);

            SocketMsg msg = sock.nextMsg();
            while(msg.valid()) {
                uint8_t msgType = msg.readU8();
                editor.handleMsg(&msg, msgType); 
                msg.free();
                msg = sock.nextMsg();
            }

            if(!sock.ready()) {
                editor.free();
                runEditor = false;
                sock.free();
                initedSock = false;
            }
        } else {
            ImGui::OpenPopup("Connect to server");
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            if(ImGui::BeginPopupModal("Connect to server", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Host:");
                ImGui::SameLine();
                ImGui::InputText("##host", host, 256);
                if(ImGui::Button("Connect")) {
                    if(initedSock)
                        sock.free();
                    if(sock.init(host))
                        initedSock = true;
                }
                if(initedSock && sock.ready()) {
                    editor.init(&sock, this);
                    runEditor = true;
                }
                ImGui::EndPopup();
            }

            saveSetting("host", host);
        }

    }

    void free() {
        if(initedSock)
            sock.free();
    }

    char host[256];

    bool runEditor;
    bool initedSock;
    Socket sock;
    Editor editor;

};

int main() {
    Client client;
    runApp(&client); 
}
