
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
    void writeFwd(Arr<MsgWriter>* msgs, ProjectAction* act);
    void writeBwd(Arr<MsgWriter>* msgs, ProjectAction* act);

    Key key;
    uint8_t type;
    uint32_t objType;

    void* data;
    
    Key oldParent;
    Key newParent;

    uint32_t fieldIdx;
    void* oldData;
    void* newData;
};

class ProjectAction {
public:
    void init();
    void free();
    virtual void addOP(ProjectOP op);

    Arr<ProjectOP> ops;
};

#endif
