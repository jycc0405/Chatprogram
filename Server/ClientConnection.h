//
// Created by CS on 25. 3. 4.
//

#ifndef CHATPROGRAM_CLIENTCONNECTION_H
#define CHATPROGRAM_CLIENTCONNECTION_H

#include <string>
#include <thread>
#include <functional>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "Client.h"
#include "../ChatMessage.h"
#include "CommandDispatcher.h"

class ClientConnection {
public:
    ClientConnection(SOCKET socket, const std::string &nickname);

    ~ClientConnection();

    [[nodiscard]] SOCKET getSocket() const;

    [[nodiscard]] std::string getNickname() const;

    [[nodiscard]] bool isActive() const;

    void setInactive();

    void start(const std::function<void(const std::string &, const std::string &)> &commandDispatcher);

    void join();

private:
    Client clientData;
    std::thread threadObj;

    void handleClient(const std::function<void(const std::string &, const std::string &)> &commandDispatcher);
};

#endif //CHATPROGRAM_CLIENTCONNECTION_H
