
#include "common.h"
#include "platform.h"

void initImGui(GLFWwindow* win) {
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowMinSize = ImVec2(100, 100);
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
}

void imguiNewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imguiEndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int App::getW() {
    int w, h;
    glfwGetWindowSize(win, &w, &h);
    return w;
}

int App::getH() {
    int w, h;
    glfwGetWindowSize(win, &w, &h);
    return h;
}

glm::vec2 App::getMousePos() {
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    return glm::vec2((float)x, (float)y);
}
