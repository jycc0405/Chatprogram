//
// Created by CS on 25. 3. 8.
//

#ifndef CHATPROGRAM_CONNECTIONMANAGER_H
#define CHATPROGRAM_CONNECTIONMANAGER_H

#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

class ConnectionManager{
public:
    ConnectionManager(const std::string &ip, int port);
    ~ConnectionManager();

    bool connectToServer();
    SOCKET getSocket() const;
    void disconnect();

private:
    std::string serverIp;
    int port;
    SOCKET clientSocket;
};

#endif //CHATPROGRAM_CONNECTIONMANAGER_H
