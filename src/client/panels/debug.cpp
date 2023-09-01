
#include "debug.h"
#include "../editor.h"

void DebugPanel::init(int key) {
    Panel::init(key);
}

void DebugPanel::free() {

}

void DebugPanel::tick(Editor* editor, float dt) {
    ImGui::Text("dt: %g", dt);
    ImGui::Text("mem: %zu", anim::memUsed);
    ImGui::Text("undo: %d %d", editor->acts.acts.cnt(), editor->acts.currAct);
}

const char* DebugPanel::getName() {
    return "Debug";
}

size_t DebugPanel::getSize() {
    return sizeof(DebugPanel);
}
