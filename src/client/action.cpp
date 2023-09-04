
#include "action.h"
#include "editor.h"

void EditorAction::init(Editor* editor) {
    ProjectAction::init();
    this->editor = editor;
}

void EditorAction::addOP(ProjectOP op) {
    ProjectAction::addOP(op);
    Arr<MsgWriter> msgs;
    msgs.init();
    op.writeFwd(&msgs, this);
    for(int i = 0; i < msgs.cnt(); i++) {
        editor->sock->send(msgs[i].getData(), msgs[i].getSize());
        msgs[i].free();
    }
    msgs.free();
}



void ActionManager::init() {
    acts.init();
    currAct = -1;
}

void ActionManager::free() {
    for(int i = 0; i < acts.cnt(); i++)
        acts[i].free();
    acts.free();
}

void ActionManager::pushAction(EditorAction act) {
    // TODO: use ring buffer
    while(acts.cnt() > currAct + 1) {
        acts[acts.cnt() - 1].free();
        acts.pop(); 
    }

    acts.add(act);
    currAct++;

    if(acts.cnt() > 32) {
        acts[0].free();
        acts.removeAt(0);
        currAct--;
    }
}

bool ActionManager::hasUndo() {
    return currAct >= 0;
}

void ActionManager::undo() {
    if(!hasUndo())
        return;
    acts[currAct].undo();
    currAct--;
}

bool ActionManager::hasRedo() {
    return currAct + 1 < acts.cnt();
}

void ActionManager::redo() {
    if(!hasRedo())
        return;
    currAct++;
    acts[currAct].redo();
}
