
#include "project.h"

#include "../protocol/protocol.h"

void Project::init() {
    graphics.init();
}

void Project::free() {
    for(int i = 0; i < graphics.cnt(); i++) {
        graphics[i].free();
    }
    graphics.free();
}

void Project::loadFrom(SocketMsg* msg) {
    free();
    init();
    uint32_t nGraphics = msg->readU32();
    for(int i = 0; i < nGraphics; i++) {
        Key graphicKey = msg->readKey();
        uint32_t strokeCnt = msg->readU32();
        Graphic g;
        g.init(graphicKey);
        for(int j = 0; j < strokeCnt; j++) {
            Key strokeKey = msg->readKey();
            uint32_t nPts = msg->readU32();
            Stroke stroke;
            stroke.init(strokeKey);
            for(int k = 0; k < nPts; k++) {
                Point pt;
                Key pointKey = msg->readKey();
                glm::vec2 pos = msg->readVec2();
                pt.init(pointKey, pos);
                stroke.points.add(pt);
            }
            stroke.updateMesh();
            g.strokes.add(stroke);
        }
        graphics.add(g);
    }
}

void Project::writeTo(MsgWriter* msg) {
    msg->writeU32(graphics.cnt());
    for(int i = 0; i < graphics.cnt(); i++) {
        Graphic* g = &graphics[i];
        msg->writeKey(g->key);
        msg->writeU32(g->strokes.cnt());
        for(int j = 0; j < g->strokes.cnt(); j++) {
            Stroke* s = &g->strokes[j];
            msg->writeKey(s->key);
            msg->writeU32(s->points.cnt());
            for(int k = 0; k < s->points.cnt(); k++) {
                msg->writeKey(s->points[k].key);
                msg->writeVec2(s->points[k].pt);
            }
        }
    }
}

void Project::movePoint(Key graphicKey, Key strokeKey, Key pointKey, glm::vec2 pt, MsgWriter* msg) {
    Graphic* g = getGraphic(graphicKey);
    if(g != NULL) {
        Stroke* s = g->getStroke(strokeKey);
        if(s != NULL) {
            Point* point = s->getPoint(pointKey);
            if(point != NULL) {
                point->pt = pt;
                s->updateMesh();
            }
        }
    }

    if(msg != NULL) {
        msg->writeU8(MessageType::UPDATE);
        msg->writeU32(UpdateType::MOVE_POINT);
        msg->writeKey(graphicKey);
        msg->writeKey(strokeKey);
        msg->writeKey(pointKey);
        msg->writeVec2(pt);
    }
}

void Project::deleteStroke(Key graphicKey, Key strokeKey, MsgWriter* msg) {
    Graphic* g = getGraphic(graphicKey);
    if(g != NULL) {
        int strokeIdx = g->getStrokeIdx(strokeKey);
        if(strokeIdx != -1) {
            g->strokes[strokeIdx].free();
            g->strokes.removeAt(strokeIdx);
        }
    }

    if(msg != NULL) {
        msg->writeU8(MessageType::UPDATE);
        msg->writeU32(UpdateType::DELETE_STROKE);
        msg->writeKey(graphicKey);
        msg->writeKey(strokeKey);
    }
}

void Project::applyUpdate(SocketMsg* msg) {
    uint32_t updateType = msg->readU32();
    if(updateType == UpdateType::MOVE_POINT) {
        Key graphicKey = msg->readKey();
        Key strokeKey = msg->readKey();
        Key pointKey = msg->readKey();
        glm::vec2 pt = msg->readVec2();
        movePoint(graphicKey, strokeKey, pointKey, pt);
    }
    if(updateType == UpdateType::DELETE_STROKE) {
        Key graphicKey = msg->readKey();
        Key strokeKey = msg->readKey();
        deleteStroke(graphicKey, strokeKey);
    }
}

void Project::addStroke(Key key, Key graphicKey, MsgWriter* msg) {
    Graphic* g = getGraphic(graphicKey);
    if(g != NULL) {
        Stroke s;
        s.init(key);
        g->strokes.add(s);
    }

    if(msg != NULL) {
        msg->writeU8(MessageType::ADD_UPDATE);
        msg->writeKey(key);
        msg->writeU32(UpdateType::ADD_STROKE);
        msg->writeKey(graphicKey);
    }
}

void Project::addPointToStroke(Key key, Key graphicKey, Key strokeKey, glm::vec2 pos, MsgWriter* msg) {
    Graphic* g = getGraphic(graphicKey);
    if(g != NULL) {
        Stroke* s = g->getStroke(strokeKey);
        if(s != NULL) {
            Point pt;
            pt.init(key, pos);
            s->points.add(pt);
            s->updateMesh();
        }
    }

    if(msg != NULL) {
        msg->writeU8(MessageType::ADD_UPDATE);
        msg->writeKey(key);
        msg->writeU32(UpdateType::ADD_POINT_TO_STROKE);
        msg->writeKey(graphicKey);
        msg->writeKey(strokeKey);
        msg->writeVec2(pos);
    }
}

void Project::applyAddUpdate(SocketMsg* msg, Key key) {
    uint32_t updateType = msg->readU32();
    if(updateType == UpdateType::ADD_STROKE) {
        Key graphicKey = msg->readKey();
        addStroke(key, graphicKey);
    }
    if(updateType == UpdateType::ADD_POINT_TO_STROKE) {
        Key graphicKey = msg->readKey();
        Key strokeKey = msg->readKey();
        glm::vec2 pt = msg->readVec2();
        addPointToStroke(key, graphicKey, strokeKey, pt);
    }
}

Graphic* Project::getGraphic(Key key) {
    for(int i = 0; i < graphics.cnt(); i++)
        if(graphics[i].key == key)
            return &graphics[i];
    return NULL;
}