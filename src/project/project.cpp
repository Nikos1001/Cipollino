
#include "project.h"

#include "../protocol/protocol.h"

void Project::init() {
    strokes.init();
}

void Project::free() {
    for(int i = 0; i < strokes.cnt(); i++) {
        strokes[i].free();
    }
    strokes.free();
}

void Project::loadFrom(SocketMsg* msg) {
    free();
    init();
    uint32_t nStrokes = msg->readU32();
    for(int i = 0; i < nStrokes; i++) {
        Key strokeKey = msg->readKey();
        uint32_t nPts = msg->readU32();
        Stroke stroke;
        stroke.init(strokeKey);
        for(int j = 0; j < nPts; j++) {
            Point pt;
            Key pointKey = msg->readKey();
            glm::vec2 pos = msg->readVec2();
            pt.init(pointKey, pos);
            stroke.points.add(pt);
        }
        stroke.updateMesh();
        strokes.add(stroke);
    }
}

void Project::writeTo(MsgWriter* msg) {
    msg->writeU32(strokes.cnt());
    for(int i = 0; i < strokes.cnt(); i++) {
        Stroke* stroke = &strokes[i];
        msg->writeKey(stroke->key);
        msg->writeU32(stroke->points.cnt());
        for(int j = 0; j < stroke->points.cnt(); j++) {
            msg->writeKey(stroke->points[j].key);
            msg->writeVec2(stroke->points[j].pt);
        }
    }
}

void Project::movePoint(Key strokeKey, Key pointKey, glm::vec2 pt, MsgWriter* msg) {
    Stroke* s = getStroke(strokeKey);
    if(s != NULL) {
        Point* point = s->getPoint(pointKey);
        if(point != NULL) {
            point->pt = pt;
            s->updateMesh();
        }
    }

    if(msg != NULL) {
        msg->writeU8(MessageType::UPDATE);
        msg->writeU32(UpdateType::MOVE_POINT);
        msg->writeKey(strokeKey);
        msg->writeKey(pointKey);
        msg->writeVec2(pt);
    }
}

void Project::deleteStroke(Key strokeKey, MsgWriter* msg) {
    int strokeIdx = getStrokeIdx(strokeKey);
    if(strokeIdx != -1) {
        strokes[strokeIdx].free();
        strokes.removeAt(strokeIdx);
    }

    if(msg != NULL) {
        msg->writeU8(MessageType::UPDATE);
        msg->writeU32(UpdateType::DELETE_STROKE);
        msg->writeKey(strokeKey);
    }
}

void Project::applyUpdate(SocketMsg* msg) {
    uint32_t updateType = msg->readU32();
    if(updateType == UpdateType::MOVE_POINT) {
        Key strokeKey = msg->readKey();
        Key pointKey = msg->readKey();
        glm::vec2 pt = msg->readVec2();
        movePoint(strokeKey, pointKey, pt);
    }
    if(updateType == UpdateType::DELETE_STROKE) {
        Key strokeKey = msg->readKey();
        deleteStroke(strokeKey);
    }
}

void Project::addStroke(Key key, MsgWriter* msg) {
    Stroke s;
    s.init(key);
    strokes.add(s);

    if(msg != NULL) {
        msg->writeU8(MessageType::ADD_UPDATE);
        msg->writeKey(key);
        msg->writeU32(UpdateType::ADD_STROKE);
    }
}

void Project::addPointToStroke(Key key, Key strokeKey, glm::vec2 pos, MsgWriter* msg) {
    Stroke* s = getStroke(strokeKey);
    if(s != NULL) {
        Point pt;
        pt.init(key, pos);
        s->points.add(pt);
        s->updateMesh();
    }

    if(msg != NULL) {
        msg->writeU8(MessageType::ADD_UPDATE);
        msg->writeKey(key);
        msg->writeU32(UpdateType::ADD_POINT_TO_STROKE);
        msg->writeKey(strokeKey);
        msg->writeVec2(pos);
    }
}

void Project::applyAddUpdate(SocketMsg* msg, Key key) {
    uint32_t updateType = msg->readU32();
    if(updateType == UpdateType::ADD_STROKE) {
        addStroke(key);
    }
    if(updateType == UpdateType::ADD_POINT_TO_STROKE) {
        Key strokeKey = msg->readKey();
        glm::vec2 pt = msg->readVec2();
        addPointToStroke(key, strokeKey, pt);
    }
}

Stroke* Project::getStroke(Key key) {
    int idx = getStrokeIdx(key);
    if(idx == -1)
        return NULL;
    return &strokes[idx];
}

int Project::getStrokeIdx(Key key) {
    for(int i = 0; i < strokes.cnt(); i++)
        if(strokes[i].key == key)
            return i; 
    return -1;
}