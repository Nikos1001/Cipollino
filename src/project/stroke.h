
#ifndef STROKE
#define STROKE

#include "../protocol/msg.h"

#ifndef SERVER
#include "../render/mesh.h"
#endif

class Point {
public:
    void init(Key key, glm::vec2 pt);

    Key key;
    glm::vec2 pt;
};

class Stroke {
public:
    void init(Key key);
    void free();

    Point* getPoint(Key key);

    Key key;
    Arr<Point> points;

#ifndef SERVER
    Mesh mesh;
#endif
    void updateMesh();
};

#endif
