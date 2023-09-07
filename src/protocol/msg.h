
#ifndef MSG_H
#define MSG_H

#include "../common/common.h"
#include "../common/dynarr.h"

#ifdef SERVER
class Client;
#endif

typedef uint64_t Key;
#define NULL_KEY UINT64_MAX

#define NAME_BUF_SIZE 64
class Name {
public:
    void init(const char* str);
    uint32_t len();
    char str[NAME_BUF_SIZE];
};

Name makeName(const char* str);

class SocketMsg {
public:
    void init();
    void free();
    bool valid();
    size_t size;
    size_t readPtr;
    void* data;

    uint8_t readU8();
    uint32_t readU32();
    float readFloat();
    Key readKey();
    Name readName();

    glm::vec2 readVec2();

#ifdef SERVER
    Client* client;
#endif
};

class MsgWriter {
public:
    void init();
    void free();
    
    void* getData();
    size_t getSize();

    void writeU8(uint8_t val);
    void writeU32(uint32_t val);
    void writeFloat(float val);
    void writeKey(Key key);
    void writeName(Name name);

    void writeVec2(glm::vec2 vec);

private:
    Arr<uint8_t> data;
};

#endif