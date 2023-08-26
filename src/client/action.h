
#ifndef ACTION_H
#define ACTION_H

#include "../common/dynarr.h"

class Editor;

class Action {
public:
    virtual void free() {}
    // returns the inverse of the action
    virtual Action* perform(Editor* editor) { return NULL; }
};

class ActionManager {
public:
    void init();
    void free();
    void addUndo(Action* undo);
    void undo(Editor* editor);
    void redo(Editor* editor);
    bool hasUndo();
    bool hasRedo();
private:
    void freeAction(Action* action);
    Arr<Action*> undos;
    Arr<Action*> redos;
};

#endif
