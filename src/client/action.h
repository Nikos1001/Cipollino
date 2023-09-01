
#ifndef EDITOR_ACTION_H
#define EDITOR_ACTION_H

#include "../project/op.h"

class Editor;

class EditorAction : public ProjectAction {
public:
    void init(Editor* editor);
    void addOP(ProjectOP op) override;
    void undo();
    void redo();
private:
    Editor* editor;
};

class ActionManager {
public:
    void init();
    void free();

    void pushAction(EditorAction act);
    
    bool hasUndo();
    void undo();
    bool hasRedo();
    void redo();

    Arr<EditorAction> acts;
    int currAct;
};

#endif
