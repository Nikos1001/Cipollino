
#ifndef PLATFORM_H
#define PLATFORM_H

#include "opengl.h"
#include "../common/dynarr.h"
#include "../common/common.h"
#include "../protocol/msg.h"

#ifndef __EMSCRIPTEN__
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>

typedef websocketpp::client<websocketpp::config::asio_tls_client> WSClient;
typedef websocketpp::config::asio_client::message_type::ptr MsgPtr;
#else
#include <emscripten/websocket.h>
#endif

class App {
public:
    virtual void init() {}
    virtual void tick(float dt) {}
    virtual void free() {}

    GLFWwindow* win;
    int getW();
    int getH();
    glm::vec2 getMousePos(); 

    void saveSetting(const char* name, const char* data);
    const char* loadSetting(const char* name);
};

void runApp(App* app);

#ifndef __EMSCRIPTEN__
struct SocketData {
    WSClient sock;
    bool ready;
    std::mutex mutex;
    websocketpp::connection_hdl server;
    Arr<SocketMsg>* msgs;
}; 
#endif

class Socket {
public:
    bool init(const char* url);
    bool ready();
    void free();

    void send(void* data, size_t size);
    SocketMsg nextMsg(); 
private:
#ifndef __EMSCRIPTEN__
    // Making this a pointer is a dirty hack to get around RAII
    std::thread sockThread;
    SocketData* sock;
#else
    EMSCRIPTEN_WEBSOCKET_T sock;
#endif
    Arr<SocketMsg>* msgs; // TODO: change to queue 
};

#endif
