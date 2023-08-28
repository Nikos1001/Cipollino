
#include "debug.h"

void DebugPanel::init(int key) {
    Panel::init(key);
}

void DebugPanel::free() {

}

void DebugPanel::tick(Editor* editor, float dt) {
    ImGui::Text("dt: %g", dt);
    ImGui::Text("mem: %zu", anim::memUsed);
}

const char* DebugPanel::getName() {
    return "Debug";
}

size_t DebugPanel::getSize() {
    return sizeof(DebugPanel);
}
