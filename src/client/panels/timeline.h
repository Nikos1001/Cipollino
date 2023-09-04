
#ifndef TIMELINE_PANEL_H
#define TIMELINE_PANEL_H

#include "../panel.h"
#include "../action.h"
#include "../select.h"

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
    Key layerCtxKey;

    bool beganEditingGfxLen;
    EditorAction setGfxLenAction;

    float frameDragX;
    EditorAction frameDragAction;
    bool beganDrag;
    bool justSelectedFrame;

    SelectionManager sels;
};

void goToPrevFrame(Editor* editor);
void goToNextFrame(Editor* editor);

#endif
