
#ifndef PANEL_H
#define PANEL_H

#include "../common/dynarr.h"
#include "../render/framebuffer.h"
#include "../render/camera.h"

class Editor;

enum PanelType {
    SCENE,
    DEBUG,

    NONE
};

class Panel {
public:
    virtual void init(int key) {
        this->key = key;
    }

    virtual void free() {}
    virtual void tick(Editor* editor, float dt) {}
    virtual const char* getName() { return "Panel"; }
    virtual size_t getSize() { return sizeof(Panel); }

    void render(Editor* editor, float dt);
    int key;
};

class PanelManager {
public:
    void init();
    void free();
    void addPanel(PanelType type);
    void tick(Editor* editor, float dt);
    void loadSettings(Editor* editor);
    void saveSettings(Editor* editor);
private:
    bool addPanelWithKey(PanelType type, int key);
    int currKey;
    Arr<Panel*> panels;
};

class ScenePanel : public Panel {
public:
    void init(int key);
    void free();
    void tick(Editor* editor, float dt);
    const char* getName();
    size_t getSize();
private:
    Framebuffer fb;
    bool prevMouseInsideViewport;
    Camera cam;
};

class DebugPanel : public Panel {
public:
    void init(int key);
    void free();
    void tick(Editor* editor, float dt);
    const char* getName();
    size_t getSize();
};

#endif
