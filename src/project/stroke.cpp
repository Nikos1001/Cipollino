
#include "project.h"
#ifndef SERVER
#include "../render/builder.h"
#endif

void Point::init(Key key, glm::vec2 pt) {
    this->key = key;
    this->pt = pt;
}

void Stroke::init(Key key) {
    this->key = key;
    points.init();
#ifndef SERVER
    mesh.init(1, (int[]){2});
    updateMesh();
#endif
}

void Stroke::free() {
    points.free();
#ifndef SERVER
    mesh.free();
#endif
}

Point* Stroke::getPoint(Key key) {
    for(int i = 0; i < points.cnt(); i++) {
        if(points[i].key == key)
            return &points[i];
    }
    return NULL;
}

#ifndef SERVER
void Stroke::updateMesh() {
    MeshBuilder builder;
    builder.init(&mesh);
    float r = 0.1f;
    for(int i = 0; i < points.cnt() - 1; i++) {
        glm::vec2 a = points[i].pt;
        glm::vec2 b = points[i + 1].pt;
        glm::vec2 aToB = glm::normalize(b - a);
        glm::vec2 norm = glm::vec2(-aToB.y, aToB.x);

        glm::vec2 v00 = a + norm * r * 0.5f;
        glm::vec2 v01 = a - norm * r * 0.5f;
        glm::vec2 v10 = b + norm * r * 0.5f;
        glm::vec2 v11 = b - norm * r * 0.5f;
        unsigned int i00 = builder.addVert((float[]){v00.x, v00.y});
        unsigned int i01 = builder.addVert((float[]){v01.x, v01.y});
        unsigned int i10 = builder.addVert((float[]){v10.x, v10.y});
        unsigned int i11 = builder.addVert((float[]){v11.x, v11.y});

        builder.addTri(i00, i11, i01);
        builder.addTri(i00, i11, i10);
    }
    builder.upload();
    builder.free();
}
#else
void Stroke::updateMesh() {

}
#endif
