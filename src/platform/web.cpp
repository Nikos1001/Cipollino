
#include "platform.h"

#include "../common/common.h"
#include "common.h"

EM_JS(int, getCanvasWidth, (), {
  return Module.canvas.clientWidth;
});

EM_JS(int, getCanvasHeight, (), {
  return Module.canvas.clientHeight;
});

App* app;
float prevTime;
int w, h;
void loop() {
    glfwPollEvents();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int currW = getCanvasWidth();
    int currH = getCanvasHeight();
    if(w != currW || h != currH || glfwGetTime() < 1) {
        w = currW;
        h = currH;
        glfwSetWindowSize(app->win, w, h);
    }

    imguiNewFrame();

    float time = glfwGetTime();
    float dt = time - prevTime;
    prevTime = time;

    app->tick(dt);

    imguiEndFrame();

    glfwSwapBuffers(app->win);
}

void runApp(App* app_) {
    app = app_;

    ASSERT(glfwInit(), "Could not init GLFW")
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    w = getCanvasWidth();
    h = getCanvasHeight();
    app->win = glfwCreateWindow(w, h, "", NULL, NULL);
    ASSERT(app->win != NULL, "Could not create GLFW window")
    glfwMakeContextCurrent(app->win);

    initImGui(app->win);

    prevTime = 0.0f;
    app->init();
    emscripten_set_main_loop(loop, 0, 1);
}

void SocketMsg::free() {
	std::free(data);
}

bool SocketMsg::valid() {
	return data != NULL;
}

EM_BOOL socketMsgCallback(int eventType, const EmscriptenWebSocketMessageEvent* event, void* usrData) {
	Arr<SocketMsg>* msgs = (Arr<SocketMsg>*)usrData;
	SocketMsg msg;
	msg.size = event->numBytes;
	msg.data = malloc(msg.size);
	memcpy(msg.data, event->data, msg.size);
	msgs->add(msg);
	return true;
}

bool Socket::init(const char* url) {
	EmscriptenWebSocketCreateAttributes attribs;
	attribs.url = url;
	attribs.createOnMainThread = true;
	attribs.protocols = NULL;
	sock = emscripten_websocket_new(&attribs);
	if(sock <= 0)
		return false;
	msgs = (Arr<SocketMsg>*)malloc(sizeof(Arr<SocketMsg>));
	msgs->init();
	EMSCRIPTEN_RESULT res = emscripten_websocket_set_onmessage_callback(sock, (void*)msgs, socketMsgCallback);
	return true;
}

bool Socket::ready() {
	unsigned short state;
	emscripten_websocket_get_ready_state(sock, &state);
	return state == 1;
}

void Socket::free() {
	emscripten_websocket_close(sock, 1000, "b'bye");
	msgs->free();
	std::free(msgs);
}

void Socket::send(void* data, size_t size) {
	emscripten_websocket_send_binary(sock, data, size);
}

SocketMsg Socket::nextMsg() {
	if(msgs->cnt() > 0) {
		SocketMsg msg = msgs->at(msgs->cnt() - 1);
		msgs->pop();
		return msg;
	}
	SocketMsg msg;
	msg.data = NULL;
	return msg;
}
