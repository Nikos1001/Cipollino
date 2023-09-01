
#include "timeline.h"
#include "../editor.h"

void TimelinePanel::init(int key) {
    Panel::init(key);
    layerInfoW = 100;
    scrollY = 0;
    scrollX = 0;
    editingName = false;
    focusNameEdit = false;
}

void TimelinePanel::free() {

}

void TimelinePanel::tick(Editor* editor, float dt) {
    Graphic* g = editor->proj.getGraphic(editor->openGraphic);
    if(g == NULL)
        return;

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    ImU32 textCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);
    ImU32 tableBorderStrongCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TableBorderStrong]);
    ImU32 tableBorderLightCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TableBorderLight]);
    ImU32 tableRowCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TableRowBg]);
    ImU32 tableRowAltCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TableRowBgAlt]);
    ImU32 tableHeaderCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TableHeaderBg]);
    ImU32 buttonActiveCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_ButtonActive]);

    float textH = ImGui::CalcTextSize("X").y;
    float layerH = textH + style.FramePadding.y * 3.0f;
    float totalH = g->layers.cnt() * layerH;
    float scrollScl = 5 * textH;

    float topbarH = 20;
    int frameHighlightSpacing = 5;

    // ==== Layers ====

    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, topbarH));
    ImGui::BeginChild("##timelineLayers", ImVec2(layerInfoW, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    for(int i = 0; i < g->layers.cnt(); i++) {
        ImVec2 layerNamePos = cursor + ImVec2(0, i * layerH);
        Layer* l = editor->proj.getLayer(g->layers[i]);
        if(editingName) {
            ImGui::SetCursorScreenPos(layerNamePos);
            ImGui::PushItemWidth(layerInfoW - 8.0f);
            if(focusNameEdit) {
                ImGui::SetKeyboardFocusHere(0);
                focusNameEdit = false;
            }
            if(ImGui::InputText("##layerName", l->name.str, NAME_BUF_SIZE)) {
                editor->proj.setLayerName(l->key, l->name, &layerNameEditAction);
            }
            if(ImGui::IsItemDeactivatedAfterEdit()) {
                if(l->name.len() == 0) {
                    Name name;
                    name.init("Layer");
                    editor->proj.setLayerName(l->key, name, &layerNameEditAction);
                }
                editingName = false;
                editor->acts.pushAction(layerNameEditAction);
            }
            ImGui::PopItemWidth();
        } else {
            ImGui::SetCursorScreenPos(layerNamePos + ImVec2(0, (layerH - textH) / 2.0f));
            ImGui::Text("%s", l->name.str);
            if(ImGui::IsItemHovered()) {
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    editor->activeLayer = i;
                    editingName = false;
                }
                if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    editingName = true;
                    focusNameEdit = true;
                    layerNameEditAction.init(editor);
                }
            }
        }
    }
    drawList->AddLine(cursor + ImVec2(layerInfoW - 1, 0), cursor + ImVec2(layerInfoW - 1, totalH), textCol);
    ImGui::SetCursorPos(ImVec2(0, totalH));

    bool layersHovered = ImGui::IsWindowHovered();
    if(layersHovered) {
        scrollY -= scrollScl * ImGui::GetIO().MouseWheel;
    }
    ImGui::SetScrollY(scrollY);
    ImGui::EndChild();

    // ==== Frames ====

    float frameW = 12;

    ImGui::SameLine();
    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(style.ItemSpacing.x, topbarH));
    ImGui::BeginChild("##timelineFrames", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    drawList = ImGui::GetWindowDrawList();
    cursor = ImGui::GetCursorScreenPos() + ImVec2(0, topbarH);
    ImVec2 topbarCursor = cursor + ImVec2(0, scrollY);

    uint32_t lastFrameEnd = 0;
    for(int i = 0; i < g->layers.cnt(); i++) {
        Layer* l = editor->proj.getLayer(g->layers[i]);
        for(int j = 0; j < l->frames.cnt(); j++) {
            Frame* f = editor->proj.getFrame(l->frames[j]); 
            lastFrameEnd = fmax(lastFrameEnd, f->end);
        }
    }

    float totalW = lastFrameEnd * frameW + canvasSize.x - frameW - layerInfoW;

    // Frames BG

    int lastShownFrame = lastFrameEnd + (canvasSize.x - layerInfoW) / frameW + 2;
    for(int i = 0; i < lastShownFrame; i += frameHighlightSpacing) {
        drawList->AddRectFilled(cursor + ImVec2(i * frameW, 0), cursor + ImVec2((i + frameHighlightSpacing - 1) * frameW, totalH), tableRowCol);
        drawList->AddRectFilled(cursor + ImVec2((i + frameHighlightSpacing - 1) * frameW, 0), cursor + ImVec2((i + frameHighlightSpacing) * frameW, totalH), tableRowAltCol);
    }
    if(g->layers.cnt() > 0) {
        drawList->AddRectFilled(cursor + ImVec2(0, editor->activeLayer * layerH), cursor + ImVec2(totalW, ((editor->activeLayer + 1) * layerH)), tableRowAltCol);
    }
    for(int i = 0; i < lastShownFrame; i++) {
        float xOff = i * frameW;
        drawList->AddLine(cursor + ImVec2(xOff, 0), cursor + ImVec2(xOff, totalH), tableBorderLightCol);
    }

    // Frames

    for(int i = 0; i < g->layers.cnt(); i++) {
        Layer* l = editor->proj.getLayer(g->layers[i]);
        for(int j = 0; j < l->frames.cnt(); j++) {
            Frame* f = editor->proj.getFrame(l->frames[j]);
            drawList->AddRectFilled(cursor + ImVec2(f->begin * frameW, i * layerH), cursor + ImVec2((f->end + 1) * frameW, (i + 1) * layerH), tableHeaderCol);
            drawList->AddRect(cursor + ImVec2(f->begin * frameW, i * layerH), cursor + ImVec2((f->end + 1) * frameW, (i + 1) * layerH), tableBorderStrongCol);
            drawList->AddCircleFilled(cursor + ImVec2((f->begin + 0.5f) * frameW, i * layerH + 0.5f * layerH), frameW * 0.25f, textCol);
            if(f->begin != f->end) {
                drawList->AddRect(cursor + ImVec2((f->end + 0.3f) * frameW, (i + 0.3f) * layerH), cursor + ImVec2((f->end + 0.7f) * frameW, (i + 0.7f) * layerH), textCol);
            }
        }
    }

    int currFrame = (int)(editor->time / (1.0f / editor->proj.fps));
    drawList->AddLine(cursor + ImVec2((currFrame + 0.5f) * frameW, 0), cursor + ImVec2((currFrame + 0.5f) * frameW, totalH), buttonActiveCol, 2);
    

    // ==== Top bar ====

    drawList->AddRectFilled(topbarCursor + ImVec2(0, -topbarH - 1), topbarCursor + ImVec2(totalW, 0), tableHeaderCol);
    drawList->AddLine(topbarCursor, topbarCursor + ImVec2(totalW, 0), textCol);
    for(int i = frameHighlightSpacing; i < lastShownFrame; i += frameHighlightSpacing) {
        char buf[64];
        snprintf(buf, 64, "%d", i);
        ImGui::SetCursorScreenPos(topbarCursor + ImVec2((i - 1 + 0.5f) * frameW - ImGui::CalcTextSize(buf).x / 2.0f, -topbarH + 2.0f));
        ImGui::Text("%s", buf);
    }

    if(ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        ImVec2 pos = ImGui::GetMousePos() - cursor;
        if(pos.y < scrollY) {
            editor->time = pos.x / frameW * (1.0f / editor->proj.fps);
            editor->playing = false;
        }
    }

    ImGui::SetCursorPos(ImVec2(totalW, totalH));
    if(ImGui::IsWindowHovered()) {
        scrollX -= scrollScl * ImGui::GetIO().MouseWheelH;
        scrollY -= scrollScl * ImGui::GetIO().MouseWheel;
        ImGui::SetScrollY(scrollY);
        ImGui::SetScrollX(scrollX);
    } else if(layersHovered) {
        ImGui::SetScrollY(scrollY);
    } else {
        scrollY = ImGui::GetScrollY();
        scrollX = ImGui::GetScrollX();
    }

    scrollX = fmax(scrollX, 0.0f);
    scrollX = fmin(scrollX, ImGui::GetScrollMaxX());
    scrollY = fmax(scrollY, 0.0f);
    scrollY = fmin(scrollY, ImGui::GetScrollMaxY());

    ImGui::EndChild();

    
}

const char* TimelinePanel::getName() {
    return "Timeline";
}

size_t TimelinePanel::getSize() {
    return sizeof(TimelinePanel);
}
