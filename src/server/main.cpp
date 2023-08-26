
#include "../common/common.h"
#include "../project/project.h"
#include "../protocol/msg.h"
#include "../protocol/protocol.h"
#include "server.h"
#include "keysmith.h"

int main() {
    printf("Hello from the server!\n");

    Project proj;
    proj.init();
    Stroke s;
    s.init(0);
    Point pt;
    pt.init(1, glm::vec2(0.0f, 0.0f));
    s.points.add(pt);
    pt.init(2, glm::vec2(1.0f, 0.0f));
    s.points.add(pt);
    proj.strokes.add(s);

    Server server;
    if(!server.init(2000)) {
        printf("Server failed to initialize.\n");
        return 1;
    }

    KeySmith keys;
    keys.init(1000);

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
            proj.applyUpdate(&msg);
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
    }

    return 0;
}
