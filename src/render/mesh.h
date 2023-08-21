
#ifndef MESH_H
#define MESH_H

#define MAX_NUM_ATTRIBS 16

class Mesh {
public:
    void init(int nAttribs, int* attribSize);
    void upload();
    void render();

    int verts;
    float* vertData;
    int tris;
    unsigned int* idxs;
private:
    void configAttribs();

    unsigned int vbo;
    unsigned int ebo;
    unsigned int vao;
    int nAttribs;
    int attribSize[MAX_NUM_ATTRIBS];
    unsigned int floatsPerVert;
};

#endif
