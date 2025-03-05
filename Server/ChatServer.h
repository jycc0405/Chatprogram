//
// Created by CS on 25. 3. 5.
//

#ifndef CHATPROGRAM_CHATSERVER_H
#define CHATPROGRAM_CHATSERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>

#include "Client.h"


class ChatServer{
public:
    ChatServer();
    ~ChatServer();

private:
    int port;
    SOCKET socket;
    std::vector<Client> clients;

};

#endif //CHATPROGRAM_CHATSERVER_H
