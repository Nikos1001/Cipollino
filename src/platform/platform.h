
#ifndef PLATFORM_H
#define PLATFORM_H

#include "opengl.h"
#include "../common/dynarr.h"

#ifndef __EMSCRIPTEN__
#include "../../libs/sockpp/include/sockpp/tcp_connector.h"
#include <thread>
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

class Socket {
public:
    bool init(const char* url, int port);
    bool ready();
    void free();

    void send(void* data, size_t size);
    SocketMsg nextMsg(); 
private:
#ifndef __EMSCRIPTEN__
    // This being a pointer is a dirty hack to get around RAII
    // It is allocated with new/delete at the right times to ensure it gets constructed/desctructed properly
    sockpp::tcp_connector* sock;
    std::thread readThread;
    bool* killThread;
    std::mutex* msgMutex;
#else
    EMSCRIPTEN_WEBSOCKET_T sock;
#endif
    Arr<SocketMsg>* msgs; 
};

#endif
