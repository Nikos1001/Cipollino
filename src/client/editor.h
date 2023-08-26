
#ifndef EDITOR_H
#define EDITOR_H

#include "../project/project.h"
#include "../platform/platform.h"
#include "../render/framebuffer.h"
#include "../render/scene.h"
#include "keychain.h"
#include "tool.h"
#include "panel.h"
#include "action.h"

class Editor {
public:
    void init(Socket* sock, App* app);
    void tick(float dt);
    void handleMsg(SocketMsg* msg, uint8_t msgType);
    void free();

    KeyChain keys;
    Project proj;
    Socket* sock;
    App* app;

    ActionManager acts;

    SceneRenderer sceneRndr;

    Pencil pencil;
    Tool* currTool;

    PanelManager panels;    

private:
    bool sentGet;
};

#endif
