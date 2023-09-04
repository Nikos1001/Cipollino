
#include "select.h"


void SelectionManager::init() {
    sels.init(); 
}

void SelectionManager::free() {
    sels.free();
}

void SelectionManager::clear() {
    sels.clear();
}

void SelectionManager::select(Key obj, int data) {
    if(selected(obj, data))
        return;
    Selection sel;
    sel.obj = obj;
    sel.data = data;
    sels.add(sel);
}

bool SelectionManager::selected(Key obj, int data) {
    for(int i = 0; i < sels.cnt(); i++)
        if(sels[i].obj == obj && (sels[i].data == data || data == -1))
            return true;
    return false;
}
