
#ifndef SELECT_H
#define SELECT_H

#include "../protocol/msg.h"
#include "../common/dynarr.h"

struct Selection {
    Key obj;
    int data;
};

class SelectionManager {
public:
    void init();
    void free();

    void clear();
    void select(Key obj, int data = 0);
    // If data = -1, we match all selections of the object. Otherwise, only the selections with the given data
    bool selected(Key obj, int data = -1);

    Arr<Selection> sels;
};

#endif
