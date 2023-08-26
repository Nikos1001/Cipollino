
#ifndef BUILDER_H
#define BUILDER_H

#include "mesh.h"
#include "../common/dynarr.h"

class MeshBuilder {
public:
    void init(Mesh* mesh);
    void free();
    void upload();

    unsigned int addVert(float* vert);
    void addTri(unsigned int v0, unsigned int v1, unsigned int v2);
private:
    Mesh* mesh;
    Arr<float> verts;
    Arr<unsigned int> tris;
};

#endif
