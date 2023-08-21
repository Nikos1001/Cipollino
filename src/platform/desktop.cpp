
#include "platform.h"

#include "../common/common.h"
#include "common.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void runApp(App* app) {

    ASSERT(glfwInit(), "Could not init GLFW")
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    app->win = glfwCreateWindow(1280, 720, "Anim", NULL, NULL);
    ASSERT(app->win != NULL, "Could not create GLFW window")
    glfwMakeContextCurrent(app->win);

    glewExperimental = true;
    ASSERT(glewInit() == GLEW_OK, "Could not init GLEW")

    initImGui(app->win);

    app->init();

    float prevTime = 0.0f;
    while(!glfwWindowShouldClose(app->win)) {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        imguiNewFrame();

        float time = glfwGetTime();
        float dt = time - prevTime;
        prevTime = time;

        app->tick(dt);

        imguiEndFrame();
        glfwSwapBuffers(app->win);
    }

}

void SocketMsg::free() {
    std::free(data);
}

bool SocketMsg::valid() {
    return data != NULL;
}

void socketOpenHandler(SocketData* sock, websocketpp::connection_hdl hdl) {
    sock->mutex.lock();
    sock->server = hdl;
    sock->ready = true;
    sock->mutex.unlock();
}

void socketMsgHandler(SocketData* sock, websocketpp::connection_hdl hdl, MsgPtr msgPtr) {
    SocketMsg msg;
    msg.size = msgPtr->get_payload().length();
    msg.data = malloc(msg.size);
    memcpy(msg.data, msgPtr->get_payload().c_str(), msg.size);

    sock->mutex.lock();
    sock->msgs->add(msg); 
    sock->mutex.unlock();
}

void socketThreadCode(SocketData* sock) {
    sock->mutex.lock();
    WSClient* client = &sock->sock; 
    sock->mutex.unlock();
    client->run();
}

bool Socket::init(const char* url) {
    sock = new SocketData();
    msgs = (Arr<SocketMsg>*)malloc(sizeof(Arr<SocketMsg>));
    msgs->init();
    sock->msgs = msgs;
    sock->sock.init_asio();
    sock->sock.set_open_handler(bind(socketOpenHandler, sock, ::_1));
    sock->sock.set_message_handler(bind(socketMsgHandler, sock, ::_1, ::_2));
    websocketpp::lib::error_code ec;
    WSClient::connection_ptr con = sock->sock.get_connection(url, ec);
    if(ec) {
        delete sock;
        sock = NULL;
        return false;
    }
    sock->sock.connect(con);
    std::thread t(socketThreadCode, sock); 
    sockThread = std::move(t);

    return true;
}

bool Socket::ready() {
    if(sock == NULL)
        return false;
    sock->mutex.lock();
    bool res = sock->ready;
    sock->mutex.unlock();
    return res;
}

void Socket::free() {
    while(!ready());
    sock->mutex.lock();
    websocketpp::lib::error_code ec;
    sock->sock.stop();
    sock->sock.close(sock->server, 2000, "B'bye", ec);
    sock->mutex.unlock();
    sockThread.join();
    delete sock; 
    msgs->free();
    std::free(msgs);
}

void Socket::send(void* data, size_t size) {
    while(!ready());
    sock->mutex.lock();
    sock->sock.send(sock->server, data, size, websocketpp::frame::opcode::BINARY);
    sock->mutex.unlock();
}

SocketMsg Socket::nextMsg() {
    sock->mutex.lock();
    if(msgs->cnt() > 0) {
        SocketMsg msg = msgs->at(msgs->cnt() - 1);
        msgs->pop();
        sock->mutex.unlock();
        return msg;
    }
    sock->mutex.unlock();
    SocketMsg msg;
    msg.data = NULL;
    return msg;
}
