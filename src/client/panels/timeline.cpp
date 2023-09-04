
#include "timeline.h"
#include "../editor.h"
#include "../gui.h"

void TimelinePanel::init(int key) {
    Panel::init(key);
    layerInfoW = 100;
    scrollY = 0;
    scrollX = 0;
    editingName = false;
    focusNameEdit = false;
    beganEditingGfxLen = false;
    frameDragX = 0;
    beganDrag = false;
    justSelectedFrame = false;
    sels.init();
}

void TimelinePanel::free() {
    sels.free();
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
    ImU32 emptyFrameFillCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImU32 buttonActiveCol = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_ButtonActive]);
    ImU32 selectedFrameFillCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.3f, 0.8f, 0.3f));
    ImU32 endOfGraphicDarkenCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 0.6f));

    float textH = ImGui::CalcTextSize("X").y;
    float layerH = textH + style.FramePadding.y * 3.0f;
    float totalH = g->layers.cnt() * layerH;
    float scrollScl = 5 * textH;
    float totalScrollbarW = style.ScrollbarSize + 2 * style.FramePadding.y;

    float topbarH = 20;
    float controlBarH = textH + 2 * style.FramePadding.y + style.WindowPadding.y;
    int frameHighlightSpacing = 5;

    ImVec2 mousePos = ImGui::GetMousePos();

    // ==== Control Bar ====

    ImVec2 initCursor = ImGui::GetCursorPos();
    ImGui::BeginChild("controlBar");
    ImVec2 cursor = ImGui::GetCursorPos();

    if(ImGui::Button("+")) {
        EditorAction addLayer;
        addLayer.init(editor);
        Name name;
        name.init("Layer");
        editor->proj.addLayer(editor->keys.nextKey(), g->key, name, &addLayer);
        editor->acts.pushAction(addLayer);
    }

    ImGui::SameLine(0, 20);
    if(ImGui::Button("<<"))
        editor->setTime(0.0f);
    ImGui::SameLine(0, 2);
    if(ImGui::Button("*<"))
        goToPrevFrame(editor);
    ImGui::SameLine(0, 2);
    if(ImGui::Button(editor->playing ? "||" : ">", ImVec2(ImGui::CalcTextSize("||").x + 2 * style.FramePadding.x, 0)))
        editor->playing = !editor->playing;
    ImGui::SameLine(0, 2);
    if(ImGui::Button(">*"))
        goToNextFrame(editor);
    ImGui::SameLine(0, 2);
    if(ImGui::Button(">>"))
        editor->setTime((g->len - 1) * editor->proj.frameLen());

    ImGui::SameLine(0, 20);
    int len = g->len;
    ImGui::PushItemWidth(100);
    if(ImGui::DragInt("##graphicLen", &len, 1, 1, 99999, "Length: %d", ImGuiSliderFlags_AlwaysClamp) && len != g->len) {
        if(!beganEditingGfxLen) {
            setGfxLenAction.init(editor);
            beganEditingGfxLen = true;
        } 
        editor->proj.setGraphicLen(g->key, len, &setGfxLenAction);
    }
    if(ImGui::IsItemDeactivated()) {
        editor->acts.pushAction(setGfxLenAction);
        beganEditingGfxLen = false;
    }
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), controlBarH));
    ImGui::EndChild();
    ImGui::SetCursorPos(initCursor);

    // ==== Layers ====

    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, topbarH + controlBarH));
    ImGui::BeginChild("##timelineLayers", ImVec2(layerInfoW, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    cursor = ImGui::GetCursorScreenPos();

    if(g->layers.cnt() > 0) {
        drawList->AddRectFilled(cursor + ImVec2(0, editor->activeLayer * layerH), cursor + ImVec2(layerInfoW, (editor->activeLayer + 1) * layerH), tableRowAltCol);
    }

    if(ImGui::BeginPopup("##layerContext")) {
        if(ImGui::MenuItem("Rename")) {
            editingName = true;
            focusNameEdit = true;
            for(int i = 0; i < g->layers.cnt(); i++)
                if(g->layers[i] == layerCtxKey)
                    editor->activeLayer = i;
        }
        if(ImGui::MenuItem("Delete")) {
            EditorAction deleteLayer;
            deleteLayer.init(editor);
            editor->proj.deleteLayer(layerCtxKey, &deleteLayer);
            editor->acts.pushAction(deleteLayer);
        }
        ImGui::EndPopup();
    }

    for(int i = 0; i < g->layers.cnt(); i++) {
        ImVec2 tl = cursor + ImVec2(0, i * layerH);
        ImVec2 br = tl + ImVec2(layerInfoW, layerH);
        ImVec2 layerNamePos = cursor + ImVec2(2, i * layerH);
        Layer* l = editor->proj.getLayer(g->layers[i]);
        if(editingName && i == editor->activeLayer) {
            ImGui::SetCursorScreenPos(layerNamePos);
            ImGui::PushItemWidth(layerInfoW - 8.0f);
            if(focusNameEdit) {
                ImGui::SetKeyboardFocusHere(0);
                focusNameEdit = false;
            }
            Name temp;
            memcpy(temp.str, l->name.str, NAME_BUF_SIZE);
            if(ImGui::InputText("##layerName", temp.str, NAME_BUF_SIZE)) {
                editor->proj.setLayerName(l->key, temp, &layerNameEditAction);
            }
            if(ImGui::IsItemDeactivated()) {
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
            ImU32 col = i == editor->activeLayer ? buttonActiveCol : textCol;
            ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(col), "%s", l->name.str);
            if(mousePos.x >= tl.x && mousePos.x <= br.x && mousePos.y >= tl.y && mousePos.y <= br.y) {
                

                if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    editor->activeLayer = i;
                    editingName = false;
                }
                if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    editor->activeLayer = i;
                    editingName = false;
                    ImGui::OpenPopup("##layerContext");
                    layerCtxKey = l->key;
                }
                if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    editingName = true;
                    focusNameEdit = true;
                    layerNameEditAction.init(editor);
                }
            }
        }
    }
    drawList->AddLine(cursor + ImVec2(layerInfoW - 1, 0), cursor + ImVec2(layerInfoW - 1, totalH + totalScrollbarW), textCol);
    ImGui::SetCursorPos(ImVec2(0, totalH + totalScrollbarW));

    bool layersHovered = ImGui::IsWindowHovered();
    if(layersHovered) {
        scrollY -= scrollScl * ImGui::GetIO().MouseWheel;
    }
    scrollY = fmax(scrollY, 0.0f);
    scrollY = fmin(scrollY, ImGui::GetScrollMaxY());
    ImGui::SetScrollY(scrollY);
    ImGui::EndChild();

    // ==== Frames ====

    float frameW = 12;

    ImGui::SameLine();
    ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(style.ItemSpacing.x, topbarH));
    ImGui::BeginChild("##timelineFrames", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    drawList = ImGui::GetWindowDrawList();
    cursor = ImGui::GetCursorScreenPos() + ImVec2(0, topbarH);
    ImVec2 topbarCursor = cursor + ImVec2(0, ImGui::GetScrollY());

    uint32_t lastFrame = g->len;

    float totalW = lastFrame * frameW + canvasSize.x - frameW - layerInfoW;

    // Frames BG

    int lastShownFrame = lastFrame + (canvasSize.x - layerInfoW) / frameW + 2;
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

    bool frameAreaHovered = mousePos.x >= cursor.x && mousePos.x <= cursor.x + totalW && mousePos.y >= cursor.y + 0 && mousePos.y <= cursor.y + ImGui::GetContentRegionAvail().y - topbarH; 
    bool frameAreaClicked = frameAreaHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left); 
    bool frameAreaReleased = frameAreaHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left); 
    bool dragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

    if(frameAreaClicked) {
        frameDragX = 0;
    }
    if(dragging) {
        frameDragX += ImGui::GetIO().MouseDelta.x;
        int move = 0;
        while(frameDragX > frameW) {
            move++;
            frameDragX -= frameW;
        }
        while(frameDragX < -frameW) {
            move--;
            frameDragX += frameW;
        }
        if(move != 0 && (frameAreaHovered || beganDrag)) {
            if(!beganDrag) {
                frameDragAction.init(editor);
                beganDrag = true;
            }
            for(int i = 0; i < sels.sels.cnt(); i++) {
                Selection sel = sels.sels[i];
                Frame* f = editor->proj.getFrame(sel.obj);
                if(f == NULL)
                    continue;
                if((int)f->begin + move >= 0)
                    editor->proj.setFrameBegin(f->key, f->begin + move, &frameDragAction);
            }
        }
    }
    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if(beganDrag) {
            for(int i = 0; i < sels.sels.cnt(); i++) {
                Selection sel = sels.sels[i];
                Frame* f = editor->proj.getFrame(sel.obj);
                Layer* l = editor->proj.getLayer(f->layer);
                for(int j = l->frames.cnt() - 1; j >= 0; j--) {
                    Frame* other = editor->proj.getFrame(l->frames[j]);
                    if(f->begin == other->begin && other->key != f->key) {
                        editor->proj.deleteFrame(other->key, &frameDragAction);
                    }
                }
            }
            editor->acts.pushAction(frameDragAction);
            beganDrag = false;
        } else if(!shiftDown()) {
            if(sels.sels.cnt() > 0) {
                Selection lastSel = sels.sels[sels.sels.cnt() - 1];
                sels.sels.clear();
                if(justSelectedFrame) {
                    sels.sels.add(lastSel);
                }
            }
        }
        justSelectedFrame = false;
    }
    if(canDoShortcuts() && ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        EditorAction deleteFrames;
        deleteFrames.init(editor);
        for(int i = 0; i < sels.sels.cnt(); i++)
            editor->proj.deleteFrame(sels.sels[i].obj, &deleteFrames);
        editor->acts.pushAction(deleteFrames);
        sels.clear();
    }

    for(int i = 0; i < g->layers.cnt(); i++) {
        Layer* l = editor->proj.getLayer(g->layers[i]);
        for(int j = 0; j < l->frames.cnt(); j++) {
            Frame* f = editor->proj.getFrame(l->frames[j]);
            Frame* nextF = l->getFrameAfter(&editor->proj, f->begin);
            ImVec2 tl = cursor + ImVec2(f->begin * frameW, i * layerH);
            float brX = (nextF == NULL ? (f->begin >= g->len ? (f->begin + 1) * frameW : g->len * frameW) : nextF->begin * frameW);
            ImVec2 br = cursor + ImVec2(brX, (i + 1) * layerH);
            drawList->AddRectFilled(tl, br, f->empty() ? emptyFrameFillCol : tableHeaderCol);
            drawList->AddRect(tl, br, tableBorderStrongCol);
            ImVec2 frameCircleCenter = cursor + ImVec2((f->begin + 0.5f) * frameW, i * layerH + 0.5f * layerH);
            float frameCircleR = frameW * 0.25f;
            if(!f->empty())
                drawList->AddCircleFilled(frameCircleCenter, frameCircleR, textCol);
            else
                drawList->AddCircle(frameCircleCenter, frameCircleR, tableBorderStrongCol, 0, 1.0f);

            bool selected = sels.selected(f->key, 0);
            if(selected) {
                drawList->AddRect(tl, ImVec2(tl.x + frameW, br.y), buttonActiveCol);
                drawList->AddRectFilled(tl, ImVec2(tl.x + frameW, br.y), selectedFrameFillCol);
            }

            if(frameAreaClicked) {
                if(mousePos.y >= tl.y && mousePos.y <= br.y) {
                    if(mousePos.x >= tl.x && mousePos.x <= tl.x + frameW) {
                        sels.select(f->key);
                        justSelectedFrame = true;
                    }
                }
            }

        }
    }

    // ==== Top bar ====

    drawList->AddRectFilled(topbarCursor + ImVec2(0, -topbarH - 1), topbarCursor + ImVec2(totalW + 20.0f, 0), tableHeaderCol);
    drawList->AddLine(topbarCursor, topbarCursor + ImVec2(totalW, 0), textCol);
    for(int i = frameHighlightSpacing; i < lastShownFrame; i += frameHighlightSpacing) {
        char buf[64];
        snprintf(buf, 64, "%d", i);
        ImGui::SetCursorScreenPos(topbarCursor + ImVec2((i - 1 + 0.5f) * frameW - ImGui::CalcTextSize(buf).x / 2.0f, -topbarH + 2.0f));
        ImGui::Text("%s", buf);
    }
    drawList->AddRectFilled(topbarCursor + ImVec2(g->len * frameW, -topbarH - 1), topbarCursor + ImVec2(totalW + 20.0f, 0), endOfGraphicDarkenCol);

    if(ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        ImVec2 pos = ImGui::GetMousePos() - cursor;
        if(pos.y < scrollY) {
            editor->time = pos.x / frameW * (1.0f / editor->proj.fps);
            editor->playing = false;
        }
    }

    ImGui::SetCursorPos(ImVec2(totalW, totalH + totalScrollbarW));
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

        
    int currFrame = (int)(editor->time / (1.0f / editor->proj.fps));
    float playheadX = (currFrame + 0.5f) * frameW;
    drawList->AddLine(cursor + ImVec2(playheadX, -9), cursor + ImVec2(playheadX, totalH), buttonActiveCol, 2);
    drawList->AddRectFilled(topbarCursor + ImVec2(playheadX - 3, -topbarH), topbarCursor + ImVec2(playheadX + 4, -8), buttonActiveCol);
    if(editor->playing) {
        if(playheadX - scrollX > canvasSize.x - layerInfoW)
            scrollX += canvasSize.x - layerInfoW;
        if(playheadX - scrollX < 0)
            scrollX -= canvasSize.x - layerInfoW;
    }

    drawList->AddRectFilled(cursor + ImVec2(g->len * frameW, 0), cursor + ImVec2(totalW + 20.0f, totalH), endOfGraphicDarkenCol);

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

void goToPrevFrame(Editor* editor) {
    Graphic* g = editor->proj.getGraphic(editor->openGraphic);
    if(g == NULL || g->layers.cnt() == 0)
        return;
    Layer* l = editor->proj.getLayer(g->layers[editor->activeLayer]);
    Frame* frameBefore = l->getFrameAt(&editor->proj, editor->getFrame() - 1);
    if(frameBefore != NULL)
        editor->setTime(frameBefore->begin * editor->proj.frameLen());
}

void goToNextFrame(Editor* editor) {
    Graphic* g = editor->proj.getGraphic(editor->openGraphic);
    if(g == NULL || g->layers.cnt() == 0)
        return;
    Layer* l = editor->proj.getLayer(g->layers[editor->activeLayer]);
    Frame* frameAfter = l->getFrameAfter(&editor->proj, editor->getFrame());
    if(frameAfter != NULL)
        editor->setTime(frameAfter->begin * editor->proj.frameLen());
}
