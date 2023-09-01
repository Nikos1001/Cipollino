
#include "msg.h"
#include <cstdlib>

void Name::init(const char* str) {
    this->str[NAME_BUF_SIZE - 1] = '\0';
    int len = fmin(strlen(str), NAME_BUF_SIZE - 1);
    for(int i = 0; i < len; i++)
        this->str[i] = str[i];
}

uint32_t Name::len() {
    return strlen(str);
}

bool littleEndian() {
    unsigned int x = 1;
    char* c = (char*)&x;
    return (int)*c == 1;
}

void SocketMsg::init() {
    size = 0;
    data = NULL; 
    readPtr = 0;
}

void SocketMsg::free() {
    anim::free(data, size);
}

bool SocketMsg::valid() {
    return data != NULL;
}

uint8_t SocketMsg::readU8() {
    readPtr++;
    if(readPtr > size)
        return 255;
    return ((uint8_t*)data)[readPtr - 1];
}   

uint32_t SocketMsg::readU32() {
    uint8_t b0, b1, b2, b3;
    if(littleEndian()) {
        b0 = readU8(); 
        b1 = readU8(); 
        b2 = readU8(); 
        b3 = readU8(); 
    } else {
        b3 = readU8(); 
        b2 = readU8(); 
        b1 = readU8(); 
        b0 = readU8(); 
    }
    return (b0 << 0) |
           (b1 << 8) |
           (b2 << 16) |
           (b3 << 24);
}

float SocketMsg::readFloat() {
    // important todo: fix endianness!!!
    uint8_t buf[4];
    buf[0] = readU8();
    buf[1] = readU8();
    buf[2] = readU8();
    buf[3] = readU8();
    return *((float*)buf);
}

Key SocketMsg::readKey() {
    uint64_t buf[sizeof(uint64_t)];
    for(int i = 0; i < sizeof(uint64_t); i++)
        buf[i] = readU8();
    uint64_t key = 0;
    if(littleEndian()) {
        for(int i = 0; i < sizeof(uint64_t); i++)
            key |= buf[i] << (8 * i);
    } else {
        for(int i = 0; i < sizeof(uint64_t); i++)
            key |= buf[sizeof(uint64_t) - i - 1] << (8 * i);
    }
    return key;
}

Name SocketMsg::readName() {
    uint32_t len = readU32();
    len = fmin(len, NAME_BUF_SIZE - 1);
    Name name;
    for(int i = 0; i < len; i++)
        name.str[i] = readU8();
    name.str[len] = '\0';
    return name;
}

glm::vec2 SocketMsg::readVec2() {
    float x = readFloat();
    float y = readFloat();
    return glm::vec2(x, y);
}



void MsgWriter::init() {
    data.init();
}

void MsgWriter::free() {
    data.free();
}

void* MsgWriter::getData() {
    return (void*)data.elems;
}

size_t MsgWriter::getSize() {
    return data.cnt();
}

void MsgWriter::writeU8(uint8_t u8) {
    data.add(u8);
}

void MsgWriter::writeU32(uint32_t u32) {
    uint8_t b0 = (u32 >> 0) & 0xFF;
    uint8_t b1 = (u32 >> 8) & 0xFF;
    uint8_t b2 = (u32 >> 16) & 0xFF;
    uint8_t b3 = (u32 >> 24) & 0xFF;
    if(littleEndian()) {
        writeU8(b0);
        writeU8(b1);
        writeU8(b2);
        writeU8(b3);
    } else {
        writeU8(b3);
        writeU8(b2);
        writeU8(b1);
        writeU8(b0);
    }
}

void MsgWriter::writeFloat(float val) {
    // important todo: fix endiannes!
    uint8_t buf[4];
    *((float*)buf) = val; 
    writeU8(buf[0]);
    writeU8(buf[1]);
    writeU8(buf[2]);
    writeU8(buf[3]);
}

void MsgWriter::writeKey(Key key) {
    uint8_t buf[sizeof(uint64_t)];
    for(int i = 0; i < sizeof(uint64_t); i++) {
        if(littleEndian()) {
            buf[i] = key & 0xFF;
        } else {
            buf[sizeof(uint64_t) - i - 1] = key & 0xFF;
        }
        key >>= 8;
    }
    for(int i = 0; i < sizeof(uint64_t); i++)
        writeU8(buf[i]);
}

void MsgWriter::writeName(Name name) {
    uint32_t len = strlen(name.str);
    writeU32(len);
    for(int i = 0; i < len; i++)
        writeU8(name.str[i]); 
}

void MsgWriter::writeVec2(glm::vec2 vec) {
    writeFloat(vec.x);
    writeFloat(vec.y);
}
