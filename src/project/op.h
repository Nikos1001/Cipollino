
#ifndef PROJECT_ACTION_H
#define PROJECT_ACTION_H

#include "../common/common.h"
#include "../common/dynarr.h"
#include "../protocol/msg.h"

class Project;
class ProjectAction;

class ProjectOP {
public:
    void free();
    void writeFwd(MsgWriter* msg, ProjectAction* act);
    void writeBwd(MsgWriter* msg, ProjectAction* act);

    int key;
    uint8_t type;
    uint32_t objType;

    void* data;
    
    int oldParent;
    int newParent;

    uint32_t fieldIdx;
    void* oldData;
    void* newData;
};

class ProjectAction {
public:
    void init();
    void free();
    virtual void addOP(ProjectOP op);
    int getKey(Key key);

    Arr<Key> keys;
    Arr<ProjectOP> ops;
};

#endif
