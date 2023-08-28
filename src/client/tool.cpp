
#include "tool.h"
#include "editor.h"

class PencilUndo : public Action {
public:
    void init(Key graphic, Key stroke);
    void free();
    Action* perform(Editor* editor);
private:
    Key graphic, stroke;
};

class PencilRedo : public Action {
public:
    void init(Editor* editor, Key graphic, Key strokeKey);
    void free();
    Action* perform(Editor* editor);
private:
    Arr<glm::vec2> pts;
    Key graphic, stroke;
};

void PencilUndo::init(Key graphic, Key stroke) {
    this->graphic = graphic;
    this->stroke = stroke;
}

void PencilUndo::free() {

}

Action* PencilUndo::perform(Editor* editor) {
    PencilRedo* redo = new PencilRedo();
    redo->init(editor, graphic, stroke);

    MsgWriter msg;
    msg.init();
    editor->proj.deleteStroke(graphic, stroke, &msg);
    editor->sock->send(msg.getData(), msg.getSize());
    msg.free();

    return (Action*)redo;
}

void PencilRedo::init(Editor* editor, Key graphic, Key strokeKey) {
    this->graphic = graphic;
    pts.init();
    Stroke* s = editor->proj.getGraphic(graphic)->getStroke(strokeKey);
    for(int i = 0; i < s->points.cnt(); i++) {
        pts.add(s->points[i].pt);
    } 
    stroke = NULL_KEY;
}

void PencilRedo::free() {
    pts.free();
}

Action* PencilRedo::perform(Editor* editor) {
    MsgWriter msg;
    msg.init();
    stroke = editor->keys.nextKey();
    editor->proj.addStroke(graphic, stroke, &msg);
    editor->sock->send(msg.getData(), msg.getSize());
    msg.free();

    for(int i = 0; i < pts.cnt(); i++) {
        Key pt = editor->keys.nextKey();
        if(pt == NULL_KEY)
            break;
        msg.init();
        editor->proj.addPointToStroke(pt, graphic, stroke, pts[i], &msg);
        editor->sock->send(msg.getData(), msg.getSize());
        msg.free();
    }
    
    PencilUndo* undo = new PencilUndo();
    undo->init(graphic, stroke);
    return undo;
}



void Pencil::init() {
    currStroke = NULL_KEY;
}

void Pencil::mouseClick(Editor* editor, glm::vec2 pos) {
    MsgWriter msg;
    msg.init();
    Key k = editor->keys.nextKey();
    editor->proj.addStroke(k, editor->openGraphic, &msg);
    editor->sock->send(msg.getData(), msg.getSize());
    msg.free();
    currStroke = k; 

    MsgWriter msg2;
    msg2.init();
    Key ptKey = editor->keys.nextKey();
    editor->proj.addPointToStroke(ptKey, editor->openGraphic, k, pos, &msg2);
    editor->sock->send(msg2.getData(), msg2.getSize());
    msg2.free();
}

void Pencil::mouseDown(Editor* editor, glm::vec2 pos) {
    if(currStroke == NULL_KEY)
        return;

    Stroke* s = editor->proj.getGraphic(editor->openGraphic)->getStroke(currStroke);
    bool newPoint = false;
    if(s->points.cnt() >= 3) {
        // this basically ensures that new points are only added
        // when the brush changed directions or it moved 
        // far enough from the previous point
        glm::vec2 p0 = s->points[s->points.cnt() - 3].pt; 
        glm::vec2 p1 = s->points[s->points.cnt() - 2].pt; 
        glm::vec2 p2 = s->points[s->points.cnt() - 1].pt; 
        glm::vec2 p0p1 = glm::normalize(p1 - p0);
        glm::vec2 p1p2 = glm::normalize(p2 - p1);
        float dot = (glm::dot(p0p1, p1p2) + 1.0f) * 0.5f;
        dot = powf(dot, 300.0f);
        float newPtDist = dot * 0.5f + 0.001f;
        newPoint = glm::distance(p1, p2) > newPtDist;
    } else {
        newPoint = glm::distance(pos, s->points[s->points.cnt() - 1].pt) > 0.001f;
    }
    if(newPoint) {
        MsgWriter msg;
        msg.init();
        Key k = editor->keys.nextKey();
        editor->proj.addPointToStroke(k, editor->openGraphic, currStroke, pos, &msg);
        editor->sock->send(msg.getData(), msg.getSize());
        msg.free();
    }

    MsgWriter msg;
    msg.init();
    editor->proj.movePoint(editor->openGraphic, s->key, s->points[s->points.cnt() - 1].key, pos, &msg);
    editor->sock->send(msg.getData(), msg.getSize());
    msg.free();
}

void Pencil::mouseRelease(Editor* editor, glm::vec2 pos) {
    if(currStroke != NULL_KEY) {
        PencilUndo* undo = new PencilUndo();
        undo->init(editor->openGraphic, currStroke);
        editor->acts.addUndo(undo);
    }
    currStroke = NULL_KEY;
}
