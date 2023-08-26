
#ifndef SERVER_H
#define SERVER_H

#include "../common/dynarr.h"
#include "../common/common.h"
#include "../protocol/msg.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>

#include "keysmith.h"

typedef websocketpp::server<websocketpp::config::asio> WSServer;
typedef WSServer::message_ptr WSMsg;

class Server;

class Client {
friend class Server;
public:
    void init(Server* server, websocketpp::connection_hdl hdl);
    void send(void* data, size_t size);

    ClientKeyRecord keys; // not happy with putting this here, but oh well.
private:
    websocketpp::connection_hdl hdl;
    Server* server;
};

// DO NOT move in memory.
class Server {
friend class Client;
public:
    bool init(int port);
    SocketMsg nextMsg();
    void broadcast(void* data, size_t size, Client* except = NULL);

    void start();
    void addClient(Client* client);
    void removeClient(Client* client);
    Client* getClientByConHdl(websocketpp::connection_hdl hdl);
    void addMsg(SocketMsg msg);
private:
    WSServer server;
    std::mutex clientsMutex;
    Arr<Client*> clients;
    std::mutex msgsMutex;
    Arr<SocketMsg> msgs; // TODO: change to queue
};

#endif
