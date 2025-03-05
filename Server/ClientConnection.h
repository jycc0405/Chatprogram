//
// Created by CS on 25. 3. 4.
//

#ifndef CHATPROGRAM_CLIENTCONNECTION_H
#define CHATPROGRAM_CLIENTCONNECTION_H


#include "Client.h"
#include <string>
#include <thread>
#include <functional>

#include <winsock2.h>
#include <ws2tcpip.h>

struct ChatMessage;

class ClientConnection {
public:
    ClientConnection(SOCKET socket, const std::string &nickname);

    ~ClientConnection();

    [[nodiscard]] SOCKET getSocket() const;

    [[nodiscard]] std::string getNickname() const;

    [[nodiscard]] bool isActive() const;

    void setInactive();

    void start(const std::function<void(const ChatMessage &, SOCKET)> &broadcastFunc);

    void join();

private:
    Client clientData;
    std::thread threadObj;

    void handleClient(const std::function<void(const ChatMessage &, SOCKET)> &broadcastFunc);
};

#endif //CHATPROGRAM_CLIENTCONNECTION_H
