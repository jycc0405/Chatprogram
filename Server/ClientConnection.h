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

// 개별 클라이언트의 소켓, 닉네임 관리 및 메세지 수신 처리
class ClientConnection {
public:
    ClientConnection(SOCKET socket, const std::string &nickname);

    ~ClientConnection();

    [[nodiscard]] SOCKET getSocket() const;

    [[nodiscard]] std::string getNickname() const;

    [[nodiscard]] bool isActive() const;

    void start(const std::function<void(const ChatMessage &)> &commandDispatcher);

    void join();

private:
    Client clientData;
    std::thread threadObj;

    void handleClient(const std::function<void(const ChatMessage &)> &commandDispatcher);
};

#endif //CHATPROGRAM_CLIENTCONNECTION_H
