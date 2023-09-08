
#include "toolbar.h"
#include "../editor.h"

void ToolbarPanel::init(int key) {
    Panel::init(key);
    minSize = ImVec2(24, 24);
}

void ToolbarPanel::free() {

}

void ToolbarPanel::tick(Editor* editor, float dt) {
    ImGuiStyle& style = ImGui::GetStyle();
    float padding = 2;
    ImVec2 contentSize = ImGui::GetContentRegionAvail() + ImVec2(2 * (style.WindowPadding.x - padding), 2 * (style.WindowPadding.y - padding));
    ImVec2 cursor = ImGui::GetCursorScreenPos() + ImVec2(padding, padding) - style.WindowPadding;
    float iconSize = 30;
    ImVec2 iconSizeVec = ImVec2(iconSize, iconSize);
    float totalIconSize = iconSize + padding; 
    int iconCnt = 10;
    int iconsPerRow = fmax(contentSize.x / totalIconSize, 1); 

    auto moveCursor = [&](int idx) {
        int row = idx / iconsPerRow;
        int col = idx % iconsPerRow;
        ImGui::SetCursorScreenPos(cursor + ImVec2(col * totalIconSize, row * totalIconSize));
    };

    moveCursor(0);
    if(ImGui::Button("Pen", iconSizeVec)) {
        editor->currTool = &editor->pencil;
    }
    moveCursor(1);
    ImVec4 col = ImVec4(editor->strokeCol.x, editor->strokeCol.y, editor->strokeCol.z, editor->strokeCol.w);
    if(ImGui::ColorButton("##strokeCol", col, 0, iconSizeVec)) {
        ImGui::OpenPopup("##strokeColorPicker");
    }
    if(ImGui::BeginPopup("##strokeColorPicker")) {
        ImGui::ColorPicker4("", &editor->strokeCol.x);
        ImGui::EndPopup();
    }
}

const char* ToolbarPanel::getName() {
    return "Toolbar";
}

size_t ToolbarPanel::getSize() {
    return sizeof(ToolbarPanel);
}