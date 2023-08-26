
#include "builder.h"

void MeshBuilder::init(Mesh* mesh) {
    this->mesh = mesh;
    verts.init();
    tris.init();
}

void MeshBuilder::free() {
    verts.free();
    tris.free();
}

void MeshBuilder::upload() {
    mesh->verts = verts.cnt() / mesh->floatsPerVert;
    mesh->vertData = verts.elems;
    mesh->tris = tris.cnt() / 3;
    mesh->idxs = tris.elems;
    mesh->upload();
}

unsigned int MeshBuilder::addVert(float* vert) {
    for(int i = 0; i < mesh->floatsPerVert; i++) {
        verts.add(vert[i]);
    }
    return verts.cnt() / mesh->floatsPerVert - 1;
}

void MeshBuilder::addTri(unsigned int v0, unsigned int v1, unsigned int v2) {
    tris.add(v0);
    tris.add(v1);
    tris.add(v2);
}
