
#include "project.h"
#ifndef SERVER
#include "../render/builder.h"
#endif

#ifndef SERVER
void Stroke::updateMesh(Project* proj) {
    MeshBuilder builder;
    builder.init(&mesh);
    float r = 0.1f;

    if(points.cnt() >= 2) {
        glm::vec2 top[points.cnt()];
        glm::vec2 btm[points.cnt()];

        glm::vec2 p0 = proj->getPoint(points[0])->pt;
        glm::vec2 p0p1 = glm::normalize(proj->getPoint(points[1])->pt - p0);
        glm::vec2 p0p1N = glm::vec2(-p0p1.y, p0p1.x);
        top[0] = p0 + p0p1N * r; 
        btm[0] = p0 - p0p1N * r; 

        glm::vec2 p9 = proj->getPoint(points[points.cnt() - 1])->pt;
        glm::vec2 p8p9 = glm::normalize(p9 - proj->getPoint(points[points.cnt() - 2])->pt);
        glm::vec2 p8p9N = glm::vec2(-p8p9.y, p8p9.x);
        top[points.cnt() - 1] = p9 + p8p9N * r;
        btm[points.cnt() - 1] = p9 - p8p9N * r;
            
        for(int i = 1; i < points.cnt() - 1; i++) {
            glm::vec2 p0 = proj->getPoint(points[i - 1])->pt;
            glm::vec2 p1 = proj->getPoint(points[i])->pt;
            glm::vec2 p2 = proj->getPoint(points[i + 1])->pt;

            glm::vec2 p0p1 = glm::normalize(p1 - p0);
            glm::vec2 p0p1N = glm::vec2(-p0p1.y, p0p1.x);
            glm::vec2 p1p2 = glm::normalize(p2 - p1);
            glm::vec2 p1p2N = glm::vec2(-p1p2.y, p1p2.x);

            float normDot = glm::dot(p0p1N, p1p2N);
            float topMag = r;
            float btmMag = r;

            glm::vec2 pNorm = glm::normalize(p0p1N + p1p2N);
            top[i] = p1 + pNorm * topMag;
            btm[i] = p1 - pNorm * btmMag; 
        }

        for(int i = 0; i < points.cnt() - 1; i++) {
            glm::vec2 v00 = top[i]; 
            glm::vec2 v01 = btm[i]; 
            glm::vec2 v10 = top[i + 1]; 
            glm::vec2 v11 = btm[i + 1]; 
            unsigned int i00 = builder.addVert((float[]){v00.x, v00.y});
            unsigned int i01 = builder.addVert((float[]){v01.x, v01.y});
            unsigned int i10 = builder.addVert((float[]){v10.x, v10.y});
            unsigned int i11 = builder.addVert((float[]){v11.x, v11.y});

            builder.addTri(i00, i11, i01);
            builder.addTri(i00, i11, i10);
        }
    }
    
    builder.upload();
    builder.free();
}
#else
void Stroke::updateMesh(Project* proj) {

}
#endif
