
#ifndef PANEL_H
#define PANEL_H

#include "../common/dynarr.h"
#include "../render/framebuffer.h"
#include "../render/camera.h"

class Editor;

#define PANEL_X \
    X(Scene, SCENE) \
    X(Assets, ASSETS) \
    X(Timeline, TIMELINE) \
    X(Debug, DEBUG)

enum PanelType {
#define X(name, enumName) enumName,
    PANEL_X
#undef X

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

    bool render(Editor* editor, float dt);
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

#endif
