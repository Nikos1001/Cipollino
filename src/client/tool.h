
#ifndef TOOL_H
#define TOOL_H

#include "../common/common.h"
#include "../protocol/msg.h"
#include "action.h"

class Editor;

class Tool {
public:
    virtual void init() {}
    virtual void mouseClick(Editor* editor, glm::vec2 pos) {}
    virtual void mouseDown(Editor* editor, glm::vec2 pos) {}
    virtual void mouseRelease(Editor* editor, glm::vec2 pos) {}
};

class Pencil : public Tool {
public:
    void init();
    void mouseClick(Editor* editor, glm::vec2 pos);
    void mouseDown(Editor* editor, glm::vec2 pos);
    void mouseRelease(Editor* editor, glm::vec2 pos);
private:
    Key currStroke;
    EditorAction act;
};

#endif
