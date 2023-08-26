
#include "action.h"

void ActionManager::init() {
    undos.init();
    redos.init();
}

void ActionManager::free() {
    for(int i = 0; i < undos.cnt(); i++) {
        freeAction(undos[i]);
    }
    undos.free();
    for(int i = 0; i < redos.cnt(); i++) {
        freeAction(redos[i]);
    }
    redos.free();
}

void ActionManager::addUndo(Action* undo) {
    undos.add(undo);
    for(int i = 0; i < redos.cnt(); i++) {
        freeAction(redos[i]);
    }
    redos.clear();
}

void ActionManager::undo(Editor* editor) {
    if(undos.cnt() == 0)
        return;
    Action* undo = undos[undos.cnt() - 1]; 
    undos.pop();
    redos.add(undo->perform(editor));
    freeAction(undo);
}

void ActionManager::redo(Editor* editor) {
    if(redos.cnt() == 0)
        return;    
    Action* redo = redos[redos.cnt() - 1];
    redos.pop();
    undos.add(redo->perform(editor));
    freeAction(redo);
}

bool ActionManager::hasUndo() {
    return undos.cnt() > 0;
}

bool ActionManager::hasRedo() {
    return redos.cnt() > 0;
}

void ActionManager::freeAction(Action* action) {
    action->free();
    delete action;
}
