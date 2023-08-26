
#include "keychain.h"
#include "../protocol/protocol.h"

void KeyBlock::init() {
    curr = NULL_KEY;
    last = NULL_KEY;
}

void KeyBlock::grant(Key first, Key last) {
    curr = first;
    this->last = last;
}

Key KeyBlock::nextKey() {
    if(outOfKeys())
        return NULL_KEY;
    Key res = curr;
    curr++;
    return res;
}

bool KeyBlock::outOfKeys() {
    return curr == NULL_KEY || curr > last;
}

void KeyChain::init() {
    a.init();
    b.init(); 
    sentAReq = false;
    sentBReq = false;
    useB = false;
}

Key KeyChain::nextKey() {
    KeyBlock* primary = useB ? &b : &a; 
    KeyBlock* backup = useB ? &a : &b; 
    Key key = primary->nextKey();
    if(key != NULL_KEY) {
        return key;
    }
    useB = !useB;
    return backup->nextKey();
}

void KeyChain::sendRequests(Socket* sock) {
    if(!sock->ready())
        return;
    if(a.outOfKeys() && !sentAReq) {
        sendKeyRequest(sock);
        sentAReq = true;
    }
    if(b.outOfKeys() && !sentBReq) {
        sendKeyRequest(sock);
        sentBReq = true;
    }
}

void KeyChain::receiveKeys(SocketMsg* msg) {
    if(a.outOfKeys() && sentAReq) {
        Key first = msg->readKey();
        Key last = msg->readKey();
        a.grant(first, last);
        sentAReq = false;
        return;
    }
    if(b.outOfKeys() && sentBReq) {
        Key first = msg->readKey();
        Key last = msg->readKey();
        b.grant(first, last);
        sentBReq = false;
        return;
    }
}

void KeyChain::sendKeyRequest(Socket* sock) {
    MsgWriter msg;
    msg.init();
    msg.writeU8(MessageType::KEY_REQ);
    msg.writeU32(256);
    sock->send(msg.getData(), msg.getSize());
    msg.free();
}

bool KeyChain::outOfKeys() {
    return a.outOfKeys() && b.outOfKeys();
}
