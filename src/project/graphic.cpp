
#include "graphic.h"

void Graphic::init(Key key) {
    this->key = key;
    strokes.init();
}

void Graphic::free() {
    for(int i = 0; i < strokes.cnt(); i++) {
        strokes[i].free();
    }
    strokes.free();
}

Stroke* Graphic::getStroke(Key key) {
    int idx = getStrokeIdx(key);
    if(idx == -1)
        return NULL;
    return &strokes[idx];
}

int Graphic::getStrokeIdx(Key key) {
    for(int i = 0; i < strokes.cnt(); i++)
        if(strokes[i].key == key)
            return i; 
    return -1;
}