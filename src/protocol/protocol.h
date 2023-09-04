
#ifndef PROTOCOL_H
#define PROTOCOL_H

enum MessageType {
    // used for initial data loads of segments of project data 
    GET = 0,
    DATA = 1,

    // used for incremental updates of project data. works symmetrically between client and server
    UPDATE = 2,
    ADD_UPDATE = 3, // adding stuff needs to be its own thing to make verifying keys cleaner on the server side

    // used to request keys
    KEY_REQ = 4,
    KEY_GRANT = 5,
};

#endif
