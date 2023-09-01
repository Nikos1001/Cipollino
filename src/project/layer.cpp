
#include "project.h"

Frame* Layer::getFrameAt(Project* proj, int t) {
    for(int i = 0; i < frames.cnt(); i++) {
        Frame* f = proj->getFrame(frames[i]);
        if(t >= f->begin && t <= f->end)
            return f;
    }
    return NULL;
}

Frame* Layer::getFrameStartingAt(Project* proj, int t) {
    Frame* f = getFrameAt(proj, t);
    if(f == NULL)
        return NULL;
    if(f->begin == t)
        return f;
    return NULL;
}
