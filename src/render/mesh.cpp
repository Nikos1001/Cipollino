
#include "mesh.h"
#include "../platform/opengl.h"

void Mesh::init(int nAttribs, int* attribSize) {
    this->nAttribs = nAttribs;
    floatsPerVert = 0;
    for(int i = 0; i < nAttribs; i++) {
        this->attribSize[i] = attribSize[i];
        floatsPerVert += attribSize[i];
    }
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    verts = 0;
    vertData = NULL;
    tris = 0;
    idxs = NULL;
    upload();

    configAttribs();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void Mesh::upload() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts * floatsPerVert * sizeof(float), vertData, GL_STATIC_DRAW);
    configAttribs();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tris * 3 * sizeof(unsigned int), idxs, GL_STATIC_DRAW);
}

void Mesh::render() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    configAttribs();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    if(tris > 0)
        glDrawElements(GL_TRIANGLES, tris * 3, GL_UNSIGNED_INT, 0);
}

void Mesh::free() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::configAttribs() {
    int offset = 0;
    for(int i = 0; i < nAttribs; i++) {
        glVertexAttribPointer(i, attribSize[i], GL_FLOAT, GL_FALSE, floatsPerVert * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);  
        offset += attribSize[i];
    }
}

