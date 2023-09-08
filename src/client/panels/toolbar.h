
#ifndef TOOLBAR_PANEL_H
#define TOOLBAR_PANEL_H

#include "../panel.h"

class ToolbarPanel : public Panel {
public:
    void init(int key);
    void free();
    void tick(Editor* editor, float dt);
    const char* getName();
    size_t getSize();
};

#endif
