
#include "../common/common.h"
#include "../project/project.h"
#include "../protocol/msg.h"
#include "../protocol/protocol.h"
#include "server.h"
#include "keysmith.h"

int main() {
    printf("Hello from the server!\n");

    mkdir("proj.cipollino", S_IRWXU | S_IRWXG | S_IRWXO);

    Project proj;
    proj.init();

    KeySmith keys;
    keys.init(proj.load("proj.cipollino"));
    printf("%d\n", proj.fps);
    
    Server server;
    if(!server.init(2000)) {
        printf("Server failed to initialize.\n");
        return 1;
    }

    while(true) {
        SocketMsg msg = server.nextMsg();
        if(!msg.valid())
            continue;
        uint8_t msgType = msg.readU8();
        if(msgType == MessageType::GET) {
            MsgWriter response;
            response.init();
            response.writeU8(MessageType::DATA);
            proj.writeTo(&response); 
            msg.client->send(response.getData(), response.getSize());
            response.free();
        }
        if(msgType == MessageType::UPDATE) {
            server.broadcast(msg.data, msg.size, msg.client);
            Arr<Key> deleted;
            deleted.init();
            proj.applyUpdate(&msg, &deleted);
            for(int i = 0; i < deleted.cnt(); i++) {
                msg.client->keys.returnedKeys.add(deleted[i]);
            }
            deleted.free();
        }
        if(msgType == MessageType::ADD_UPDATE) {
            Key key = msg.readKey();
            if(msg.client->keys.useKey(key)) {
                server.broadcast(msg.data, msg.size, msg.client);
                proj.applyAddUpdate(&msg, key);
            }
        }
        if(msgType == MessageType::KEY_REQ) {
            keys.handleKeyRequest(&msg);
        }
        msg.free();

        proj.save("proj.cipollino", keys.curr);
    }

    return 0;
}
