
#ifndef KEYSMITH_H
#define KEYSMITH_H

#include "../protocol/msg.h"

class Client;
class Server;

#define MAX_KEY_GRANT_CNT 1024

struct KeyBlock {
public:
    void init();
    void grant(Key first, Key last);
    bool useKey(Key key);
    int getKeysLeft();
private:
    Key first, last;
    int keysLeft;
    bool used[MAX_KEY_GRANT_CNT];
};

class ClientKeyRecord {
friend class KeySmith;
public:
    void init(); 
    void free();
    bool useKey(Key key);
    Arr<Key> returnedKeys;
private:
    KeyBlock a;
    KeyBlock b;
};

class KeySmith {
public:
    void init(Key first);
    void handleKeyRequest(SocketMsg* msg);
private:
    Key curr;
};

#endif
