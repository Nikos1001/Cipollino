
#include "asset.h"
#include "../editor.h"

void AssetsPanel::init(int key) {
    Panel::init(key);
}

void AssetsPanel::free() {

}

void AssetsPanel::tick(Editor* editor, float dt) {
    for(int i = 0; i < editor->proj.graphics.cnt(); i++) {
        ImGui::Text("Gfx #%llu", editor->proj.graphics[i].key);
        if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            editor->openGraphic = editor->proj.graphics[i].key;
        }
    }
}

const char* AssetsPanel::getName() {
    return "Assets";
}

size_t AssetsPanel::getSize() {
    return sizeof(AssetsPanel);
}

