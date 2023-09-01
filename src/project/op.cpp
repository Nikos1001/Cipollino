
#include "op.h"

void ProjectAction::init() {
    ops.init();
    keys.init();
}

void ProjectAction::free() {
    for(int i = 0; i < ops.cnt(); i++)
        ops[i].free();
    keys.free();
}

void ProjectAction::addOP(ProjectOP op) {
    ops.add(op);
}

int ProjectAction::getKey(Key key) {
    for(int i = 0; i < keys.cnt(); i++)
        if(keys[i] == key)
            return i;
    keys.add(key);
    return keys.cnt() - 1;
}
