
#include "../common/common.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> Server;
typedef Server::message_ptr MsgPtr;

void onOpen(websocketpp::connection_hdl hdl) {
    printf("New client!\n");
}

void onMsg(websocketpp::connection_hdl hdl, MsgPtr msg) {
    printf("%d\n", msg->get_payload().length());
}

int main() {
    printf("Hello from the server!\n");

    try {
        Server server;
        server.init_asio();
        server.set_open_handler(onOpen);
        server.set_message_handler(onMsg);
        server.listen(2000);
        server.start_accept();
        server.run();
    } catch (websocketpp::exception e) {
        printf("%s\n", e.what());
    }

    return 0;
}
