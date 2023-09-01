
#ifndef TIMELINE_PANEL_H
#define TIMELINE_PANEL_H

#include "../panel.h"
#include "../action.h"

class TimelinePanel : public Panel {
public:
    void init(int key);
    void free();
    void tick(Editor* editor, float dt);
    const char* getName();
    size_t getSize();
private:
    int layerInfoW;
    float scrollY;
    float scrollX;

    bool editingName;
    bool focusNameEdit;
    EditorAction layerNameEditAction;
};



#endif
