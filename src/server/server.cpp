
#include "server.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void Client::init(Server* server, websocketpp::connection_hdl hdl) {
    this->server = server;
    memcpy(&this->hdl, &hdl, sizeof(websocketpp::connection_hdl));
    
    keys.init();
}

void Client::send(void* data, size_t size) {
    server->server.send(hdl, data, size, websocketpp::frame::opcode::BINARY);
}

void serverThread(Server* server) {
    server->start();
}

void onOpen(websocketpp::connection_hdl hdl, Server* server) {
    printf("New client!\n");
    Client* client = (Client*)anim::malloc(sizeof(Client));
    client->init(server, hdl);
    server->addClient(client);
}

void onClose(websocketpp::connection_hdl hdl, Server* server) {
    printf("Client left :(\n");
    Client* client = server->getClientByConHdl(hdl);
    server->removeClient(client); 
}

void onMsg(websocketpp::connection_hdl hdl, WSMsg wsMsg, Server* server) {
    std::string payload = wsMsg->get_payload();
    if(payload.length() > 1024 * 1024)
        return;

    SocketMsg msg;
    msg.init();
    msg.size = payload.length(); 
    msg.data = anim::malloc(msg.size);
    memcpy(msg.data, payload.c_str(), msg.size);
    msg.client = server->getClientByConHdl(hdl);

    server->addMsg(msg); 
}

bool Server::init(int port) {
    clients.init();
    msgs.init();
    try {
        server.init_asio();
        server.clear_access_channels(websocketpp::log::alevel::all);
        server.set_open_handler(bind(onOpen, ::_1, this));
        server.set_close_handler(bind(onClose, ::_1, this));
        server.set_message_handler(bind(onMsg, ::_1, ::_2, this));
        server.listen(port);
        server.start_accept();
        std::thread t(serverThread, this);
        t.detach();
    } catch(websocketpp::exception e) {
        printf("WEBSOCKET SERVER ERROR:\n%s\n", e.what());
        return false;
    }
    return true;
}

SocketMsg Server::nextMsg() {
    msgsMutex.lock();
    if(msgs.cnt() > 0) {
        SocketMsg msg = msgs[0];
        msgs.removeAt(0);
        msgsMutex.unlock();
        return msg;
    }
    msgsMutex.unlock();
    SocketMsg msg;
    msg.init();
    return msg;
}

void Server::broadcast(void* data, size_t size, Client* except) {
    clientsMutex.lock();
    for(int i = 0; i < clients.cnt(); i++) {
        if(clients[i] != except)
            clients[i]->send(data, size);
    }
    clientsMutex.unlock();
}

void Server::start() {
    server.run();
}

void Server::addClient(Client* client) {
    clientsMutex.lock();
    clients.add(client);
    clientsMutex.unlock();
}

void Server::removeClient(Client* client) {
    clientsMutex.lock();
    for(int i = 0; i < clients.cnt(); i++) {
        if(clients[i] == client) {
            clients.removeAt(i);
            break;
        }
    }
    clientsMutex.unlock();
    anim::free(client, sizeof(Client));
}

Client* Server::getClientByConHdl(websocketpp::connection_hdl hdl) {
    if(hdl.expired())
        return NULL;
    clientsMutex.lock();
    for(int i = 0; i < clients.cnt(); i++) {
        if(clients[i]->hdl.expired()) // TODO: if this is the case we should probably remove this client altogether, but this should never occur
            continue;
        if(server.get_con_from_hdl(hdl) == server.get_con_from_hdl(clients[i]->hdl)) {
            clientsMutex.unlock();
            return clients[i];
        }
    }
    clientsMutex.unlock();
    return NULL;
}

void Server::addMsg(SocketMsg msg) {
    msgsMutex.lock();
    msgs.add(msg);
    msgsMutex.unlock();
}