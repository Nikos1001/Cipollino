
#include "tool.h"
#include "editor.h"

void Pencil::init() {
    currStroke = NULL_KEY;
}

void Pencil::mouseClick(Editor* editor, glm::vec2 pos) {
    Graphic* g = editor->getOpenGraphic();
    act.init(editor);
    if(g->layers.cnt() == 0) {
        Name name;
        name.init("Layer");
        editor->proj.addLayer(editor->keys.nextKey(), g->key, name, &act);
        editor->activeLayer = 0;
    }
    Layer* l = editor->getActiveLayer(); 

    Frame* frame = editor->getActiveLayer()->getFrameStartingAt(&editor->proj, editor->getFrame());
    Key frameKey;
    if(frame == NULL) {
        frameKey = editor->keys.nextKey();
        editor->proj.addFrame(frameKey, g->layers[editor->activeLayer], editor->getFrame(), &act);
    } else {
        frameKey = frame->key;
    }

    Key k = editor->keys.nextKey();
    editor->proj.addStroke(k, frameKey, editor->strokeCol, &act);
    currStroke = k; 

    Key ptKey = editor->keys.nextKey();
    editor->proj.addPoint(ptKey, k, pos, &act);
}

void Pencil::mouseDown(Editor* editor, glm::vec2 pos) {
    if(currStroke == NULL_KEY)
        return;
    Stroke* s = editor->proj.getStroke(currStroke);

    bool newPoint = false;
    if(s->points.cnt() >= 3) {
        // this basically ensures that new points are only added
        // when the brush changed directions or it moved 
        // far enough from the previous point
        glm::vec2 p0 = editor->proj.getPoint(s->points[s->points.cnt() - 3])->pt; 
        glm::vec2 p1 = editor->proj.getPoint(s->points[s->points.cnt() - 2])->pt; 
        glm::vec2 p2 = editor->proj.getPoint(s->points[s->points.cnt() - 1])->pt; 
        glm::vec2 p0p1 = glm::normalize(p1 - p0);
        glm::vec2 p1p2 = glm::normalize(p2 - p1);
        float dot = (glm::dot(p0p1, p1p2) + 1.0f) * 0.5f;
        dot = powf(dot, 300.0f);
        float newPtDist = dot * 0.5f + 0.001f;
        newPoint = glm::distance(p1, p2) > newPtDist;
    } else {
        newPoint = glm::distance(pos, editor->proj.getPoint(s->points[s->points.cnt() - 1])->pt) > 0.001f;
    }
    if(newPoint) {
        Key k = editor->keys.nextKey();
        editor->proj.addPoint(k, currStroke, pos, &act);
    }

    editor->proj.setPointPt(s->points[s->points.cnt() - 1], pos, &act);
}

void Pencil::mouseRelease(Editor* editor, glm::vec2 pos) {
    if(currStroke != NULL_KEY) {
        editor->acts.pushAction(act);
    }
    currStroke = NULL_KEY;
}
