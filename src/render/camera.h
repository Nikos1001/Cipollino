
#ifndef CAMERA_H
#define CAMERA_H

#include "../common/common.h"

class Camera {
public:
    glm::vec2 pos;
    float size;

    glm::mat4 projView(float aspect);
};

#endif
