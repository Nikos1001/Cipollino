
#include "op.h"

void ProjectAction::init() {
    ops.init();
}

void ProjectAction::free() {
    for(int i = 0; i < ops.cnt(); i++)
        ops[i].free();
}

void ProjectAction::addOP(ProjectOP op) {
    ops.add(op);
}
