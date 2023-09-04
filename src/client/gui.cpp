
#include "gui.h"
#include "../platform/opengl.h"

bool shiftDown() {
    return ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);
}

bool ctrlDown() {
    return ImGui::GetIO().KeyMods == ImGuiMod_Super || ImGui::GetIO().KeyMods == ImGuiMod_Ctrl;
}

bool canDoShortcuts() {
    return !ImGui::GetIO().WantTextInput;
}
