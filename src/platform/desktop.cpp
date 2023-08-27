
#include "platform.h"

#include "../common/common.h"
#include "common.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

static void getSettingPath(char* buf, int size, const char* name) {
    // TODO: platforms besides macos exist!
    snprintf(buf, size, "%s/Library/Application Support/Cipollino/settings/%s.txt", getenv("HOME"), name);
    for(char* c = buf + 1; *c != '\0'; c++) {
        if(*c == '/') {
            char next = c[1];
            c[1] = '\0';
            mkdir(buf, S_IRWXU | S_IRWXG | S_IRWXO);
            c[1] = next;
        }
    }
}

void App::saveSetting(const char* name, const char* data) {
    char pathBuf[2048];
    getSettingPath(pathBuf, 2048, name);
    FILE* f = fopen(pathBuf, "w");
    fprintf(f, "%s", data);
    fclose(f);
}

const char* App::loadSetting(const char* name) {
    char pathBuf[2048];
    getSettingPath(pathBuf, 2048, name);
    FILE* f = fopen(pathBuf, "r");
    if(f == NULL)
        return NULL;

    fseek(f, 0L, SEEK_END);
    size_t fileSize = ftell(f);
    rewind(f);

    char* buffer = (char*)anim::malloc(fileSize + 1);
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, f);
    buffer[bytesRead] = '\0';

    fclose(f);

    return buffer;
}

void runApp(App* app) {

    ASSERT(glfwInit(), "Could not init GLFW")
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    app->win = glfwCreateWindow(1280, 720, "Cipollino", NULL, NULL);
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

    app->free();

}

void socketOpenHandler(SocketData* sock, websocketpp::connection_hdl hdl) {
    sock->mutex.lock();
    sock->server = hdl;
    sock->ready = true;
    sock->mutex.unlock();
}

void socketMsgHandler(SocketData* sock, websocketpp::connection_hdl hdl, MsgPtr msgPtr) {
    SocketMsg msg;
    msg.init();
    msg.size = msgPtr->get_payload().length();
    msg.data = anim::malloc(msg.size);
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
    msgs = (Arr<SocketMsg>*)anim::malloc(sizeof(Arr<SocketMsg>));
    msgs->init();
    sock->msgs = msgs;
    sock->sock.init_asio();
    sock->sock.clear_access_channels(websocketpp::log::alevel::all);
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
    anim::free(msgs, sizeof(Arr<SocketMsg>));
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
        SocketMsg msg = msgs->at(0);
        msgs->removeAt(0);
        sock->mutex.unlock();
        return msg;
    }
    sock->mutex.unlock();
    SocketMsg msg;
    msg.init();
    return msg;
}
