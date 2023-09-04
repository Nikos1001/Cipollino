
#include "keysmith.h"
#include "server.h"
#include "../protocol/protocol.h"

void KeyBlock::init() {
    first = last = NULL_KEY;
    keysLeft = 0;
}

void KeyBlock::grant(Key first, Key last) {
    this->first = first;
    this->last = last;
    keysLeft = last - first + 1;
    for(int i = 0; i < keysLeft; i++) {
        used[i] = false;
    }
}

bool KeyBlock::useKey(Key key) {
    if(first == NULL_KEY || last == NULL_KEY)
        return false;
    if(key < first || key > last)
        return false;
    int offset = key - first;
    if(used[offset])
        return false;
    used[offset] = false;
    keysLeft--;
    return true;
}

int KeyBlock::getKeysLeft() {
    return keysLeft;
}



void ClientKeyRecord::init() {
    a.init();
    b.init();
    returnedKeys.init();
}

void ClientKeyRecord::free() {
    returnedKeys.free();
}

bool ClientKeyRecord::useKey(Key key) {
    if(a.useKey(key))
        return true;
    if(b.useKey(key))
        return true;
    for(int i = 0; i < returnedKeys.cnt(); i++) {
        if(returnedKeys[i] == key) {
            returnedKeys.removeAt(i);
            return true;
        }
    }
    return false;
}


void KeySmith::init(Key first) {
    curr = first; // TODO: this is *very* important to serialize properly.
}

void KeySmith::handleKeyRequest(SocketMsg* msg) {
    uint32_t keyCnt = msg->readU32();
    if(keyCnt > MAX_KEY_GRANT_CNT)
        keyCnt = MAX_KEY_GRANT_CNT;

    Client* client = msg->client;
    bool keysGranted = false;
    Key first = curr;
    Key last = curr + keyCnt - 1;

    if(client->keys.a.getKeysLeft() == 0) {
        client->keys.a.grant(first, last);
        keysGranted = true;
    } else if(client->keys.b.getKeysLeft() == 0) {
        client->keys.b.grant(first, last);
        keysGranted = true;
    }

    if(keysGranted) {
        curr += keyCnt;
        MsgWriter response;
        response.init();
        response.writeU8(MessageType::KEY_GRANT);
        response.writeKey(first);
        response.writeKey(last);
        client->send(response.getData(), response.getSize());
        response.free();
    }
}
