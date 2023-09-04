
#include "project.h"

Frame* Layer::getFrameAt(Project* proj, int t) {
    if(t < 0)
        return NULL;    
    Frame* res = NULL;
    for(int i = 0; i < frames.cnt(); i++) {
        Frame* f = proj->getFrame(frames[i]);
        if(t >= f->begin) {
            if(res == NULL || f->begin > res->begin)
                res = f;
        }
    }
    return res;
}

Frame* Layer::getFrameStartingAt(Project* proj, int t) {
    Frame* f = getFrameAt(proj, t);
    if(f == NULL)
        return NULL;
    if(f->begin == t)
        return f;
    return NULL;
}

Frame* Layer::getFrameAfter(Project* proj, int t) {
    Frame* res = NULL;
    for(int i = 0; i < frames.cnt(); i++) {
        Frame* f = proj->getFrame(frames[i]);
        if(t < f->begin) {
            if(res == NULL || f->begin < res->begin)
                res = f;
        }
    }
    return res;
}
