
#ifndef ASSET_PANEL_H
#define ASSET_PANEL_H

#include "../panel.h"
#include "../../protocol/msg.h"
#include "../action.h"

class AssetsPanel : public Panel {
public:
    void init(int key);
    void free();
    void tick(Editor* editor, float dt);
    const char* getName();
    size_t getSize();

private:
    Key editingNameGfx;
    bool editingName;
    bool focusNameEdit;
    EditorAction nameEditAction;
    bool nameEditActionBegan;
};


#endif
