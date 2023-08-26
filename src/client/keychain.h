
#ifndef KEYCHAIN_H
#define KEYCHAIN_H

#include "../protocol/msg.h"
#include "../platform/platform.h"

class KeyBlock {
public:
    void init();
    void grant(Key first, Key last);
    Key nextKey();
    bool outOfKeys();
private:
    Key curr;
    Key last;
};

class KeyChain {
public:
    void init();
    Key nextKey();
    void sendRequests(Socket* sock);
    void receiveKeys(SocketMsg* msg);
    bool outOfKeys();
private:
    void sendKeyRequest(Socket* sock);
    KeyBlock a, b;
    bool sentAReq, sentBReq;
    bool useB;
};

#endif
