
#include "platform.h"

#include "../common/common.h"
#include "common.h"

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

void socketReadThread(bool* die, sockpp::tcp_socket sock, Arr<SocketMsg>* msgs, std::mutex* msgMutex) {
    sock.set_non_blocking(true);
    char buf[2048];
    while(!*die) {
        ssize_t n = sock.read(buf, 2048);
        if(n > 0) {
            SocketMsg msg;
            msg.size = n;
            msg.data = malloc(n);
            memcpy(msg.data, buf, n);
            msgMutex->lock();
            msgs->add(msg);
            msgMutex->unlock();
        }
    }
}

bool Socket::init(const char* url, int port) {
    sock = new sockpp::tcp_connector();
    if(!sock->connect(sockpp::inet_address(url, port))) {
        delete sock;
        sock = NULL;
        return false;
    }
    msgs = (Arr<SocketMsg>*)malloc(sizeof(Arr<SocketMsg>));
    msgs->init();

    killThread = (bool*)malloc(sizeof(bool));
    *killThread = false;

    msgMutex = new std::mutex(); 

    std::thread t(socketReadThread, killThread, std::move(sock->clone()), msgs, msgMutex);
    readThread = std::move(t);

    return true;
}

bool Socket::ready() {
    return sock != NULL;
}

void Socket::free() {
    *killThread = true;
    readThread.join();
    delete sock;
    sock = NULL;
    msgs->free();
    std::free(msgs);
    std::free(killThread);
    delete msgMutex;
}

void Socket::send(void* data, size_t size) {
    sock->write(data, size);
}

SocketMsg Socket::nextMsg() {
    msgMutex->lock(); 
    if(msgs->cnt() > 0) {
        SocketMsg msg = msgs->at(msgs->cnt() - 1);
        msgs->pop();
        msgMutex->unlock();
        return msg;
    }
    msgMutex->unlock();
    SocketMsg msg;
    msg.data = NULL;
    return msg;
}
