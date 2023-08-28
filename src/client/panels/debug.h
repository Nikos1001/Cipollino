
#ifndef DEBUG_PANEL_H
#define DEBUG_PANEL_H

#include "../panel.h"

class DebugPanel : public Panel {
public:
    void init(int key);
    void free();
    void tick(Editor* editor, float dt);
    const char* getName();
    size_t getSize();
};


#endif
