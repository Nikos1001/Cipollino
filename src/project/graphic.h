
#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "stroke.h"
#include "../protocol/msg.h"

class Graphic {
public:

    void init(Key key);
    void free();

    Key key;

    Arr<Stroke> strokes;
    Stroke* getStroke(Key key);
    int getStrokeIdx(Key key);
};

#endif
