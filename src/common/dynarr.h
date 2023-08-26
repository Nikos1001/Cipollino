
#ifndef DYNARR_H
#define DYNARR_H

#include "common.h"
template<typename T>
class Arr {
public:

    void init() {
        elems = NULL;
        size = 0;
        cap = 0;
    }

    void free() {
        anim::free((void*)elems, cap * sizeof(T));
    }

    int add(const T& elem) {
        size++;
        if(size > cap) {
            grow();
        }
        memcpy(&this->elems[size - 1], &elem, sizeof(T));
        return size - 1;
    }

    void pop() {
        size--;
    }

    void clear() {
        size = 0;
    }

    void removeAt(int i) {
        memcpy((void*)(elems + i), (void*)(elems + i + 1), sizeof(T) * (size - i - 1));
        size--;
    }

    void insertAt(int i, T elem) {
        size++;
        if(size > cap) {
            grow();
        }
        memcpy((void*)(elems + i + 1), (void*)(elems + i), sizeof(T) * (size - i - 1));
        this->elems[i] = elem;
    }

    T& at(int idx) {
        return elems[idx];
    }

    T& operator[](int idx) {
        return at(idx);
    }

    int cnt() {
        return size;
    }

    T* elems;

private:
    int size;
    int cap;
    
    void grow() {
        int newCap = cap == 0 ? 8 : cap * 2; 
        elems = (T*)anim::realloc(elems, cap * sizeof(T), newCap * sizeof(T));
        cap = newCap;
    }


};

#endif
