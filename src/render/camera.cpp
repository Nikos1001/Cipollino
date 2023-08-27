
#include "camera.h"

glm::mat4 Camera::projView(float aspect) {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-pos, 0.0f));
    glm::mat4 proj = glm::ortho(-size * aspect * 0.5f, size * aspect * 0.5f, -size * 0.5f, size * 0.5f, -1.0f, 1.0f);
    return proj * view;
} 
