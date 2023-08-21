
#ifndef PLATFORM_H
#define PLATFORM_H

#include "opengl.h"
#include "../common/dynarr.h"

#ifndef __EMSCRIPTEN__
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>

typedef websocketpp::client<websocketpp::config::asio_client> WSClient;
typedef websocketpp::config::asio_client::message_type::ptr MsgPtr;
#else
#include <emscripten/websocket.h>
#endif

class App {
public:
    virtual void init() {}
    virtual void tick(float dt) {}

    GLFWwindow* win;    
};

void runApp(App* app);

class SocketMsg {
public:
    void free();
    bool valid();
    size_t size;
    void* data;
};

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
    Arr<SocketMsg>* msgs; 
};

#endif
