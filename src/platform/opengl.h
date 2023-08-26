
#ifndef OPENGL_H
#define OPENGL_H

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLES3/gl3.h>
#else
#include "../../libs/glew/include/GL/glew.h"
#endif
#include "../../libs/glfw/include/GLFW/glfw3.h"
#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/imgui_internal.h"
#include "../../libs/imgui/backends/imgui_impl_glfw.h"
#include "../../libs/imgui/backends/imgui_impl_opengl3.h"
#include "../../libs/glm/glm/glm.hpp"

#endif
