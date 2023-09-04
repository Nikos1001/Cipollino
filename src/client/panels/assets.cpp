
#include "asset.h"
#include "../editor.h"

void AssetsPanel::init(int key) {
    Panel::init(key);
    editingName = false;
    editingNameGfx = NULL_KEY;
    focusNameEdit = false;
    nameEditActionBegan = false;
}

void AssetsPanel::free() {

}

void AssetsPanel::tick(Editor* editor, float dt) {
    if(ImGui::Button("+")) {
        Name name;
        name.init("Graphic");
        EditorAction addGraphic;
        addGraphic.init(editor);
        editor->proj.addGraphic(editor->keys.nextKey(), 100, name, &addGraphic);
        editor->acts.pushAction(addGraphic);
    }
    for(int i = 0; i < editor->proj.graphics.cnt(); i++) {
        Key key = editor->proj.graphics[i].key;
        Graphic* g = &editor->proj.graphics[i];
        if(editingName && key == editingNameGfx) {
            if(focusNameEdit) {
                ImGui::SetKeyboardFocusHere(0);
                focusNameEdit = false;
            }
            Name temp;
            temp.init(g->name.str);
            if(ImGui::InputText("##gfxName", temp.str, NAME_BUF_SIZE)) {
                if(!nameEditActionBegan) {
                    nameEditActionBegan = true;
                    nameEditAction.init(editor);
                }
                editor->proj.setGraphicName(key, temp, &nameEditAction);
            }
            if(ImGui::IsItemDeactivated()) {
                if(g->name.len() == 0) {
                    Name name;
                    name.init("Graphic");
                    if(!nameEditActionBegan) { 
                        nameEditActionBegan = true;
                        nameEditAction.init(editor);
                    }
                    editor->proj.setGraphicName(g->key, name, &nameEditAction);
                }
                editingName = false;
                editor->acts.pushAction(nameEditAction);
            }
        } else {
            ImGui::Text("%s", g->name.str);
        }
        if(ImGui::IsItemHovered()) {
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                editingName = editor->openGraphic == key;
                if(editingName) {
                    editingNameGfx = key;
                    focusNameEdit = true;
                }
                editor->openGraphic = key;
            }
        }
        if(ImGui::BeginPopupContextItem()) {
            if(ImGui::MenuItem("Rename")) {
                editingName = true;
                focusNameEdit = true;
                editingNameGfx = key;
            }
            if(ImGui::MenuItem("Delete")) {
                EditorAction deleteGraphic;
                deleteGraphic.init(editor);
                editor->proj.deleteGraphic(key, &deleteGraphic);
                editor->acts.pushAction(deleteGraphic);
            }
            ImGui::EndPopup();
        }
    }
}

const char* AssetsPanel::getName() {
    return "Assets";
}

size_t AssetsPanel::getSize() {
    return sizeof(AssetsPanel);
}

