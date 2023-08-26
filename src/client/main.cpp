
#include <stdio.h>
#include "../platform/platform.h"

#include "../protocol/protocol.h"
#include "editor.h"

class Client : public App {
    void init() {

        sock.init("ws://localhost:2000");
        editor.init(&sock, this);
    }

    void tick(float dt) {

        editor.tick(dt);

        SocketMsg msg = sock.nextMsg();
        while(msg.valid()) {
            uint8_t msgType = msg.readU8();
            editor.handleMsg(&msg, msgType); 
            msg.free();
            msg = sock.nextMsg();
        }
    }

    void free() {
        sock.free();
    }

    Socket sock;
    Editor editor;

};

int main() {
    Client client;
    runApp(&client); 
}
