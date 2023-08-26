
#ifndef PROJECT_H
#define PROJECT_H

#include "../common/common.h"
#include "../common/dynarr.h"

#include "../protocol/msg.h"

#include "stroke.h"

enum UpdateType {
    // updating
    MOVE_POINT,
    DELETE_STROKE,

    // adding
    ADD_STROKE,
    ADD_POINT_TO_STROKE
};

class Project {
public:
    void init();
    void free();

    void loadFrom(SocketMsg* msg);
    void writeTo(MsgWriter* msg);

    void movePoint(Key strokeKey, Key pointKey, glm::vec2 pt, MsgWriter* msg = NULL);
    void deleteStroke(Key strokeKey, MsgWriter* msg = NULL);
    void applyUpdate(SocketMsg* msg);

    void addStroke(Key key, MsgWriter* msg = NULL);
    void addPointToStroke(Key key, Key strokeKey, glm::vec2 pt, MsgWriter* msg = NULL);
    void applyAddUpdate(SocketMsg* msg, Key key);

    Arr<Stroke> strokes;
    Stroke* getStroke(Key key);
    int getStrokeIdx(Key key);

};

#endif
