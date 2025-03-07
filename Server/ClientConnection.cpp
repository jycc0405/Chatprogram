//
// Created by CS on 25. 3. 4.
//

#include "ClientConnection.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <cctype>

static void trim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
}

ClientConnection::ClientConnection(SOCKET socket, const std::string &nickname) {
    clientData.socket = socket;
    clientData.nickname = nickname;
    clientData.active = true;
}

ClientConnection::~ClientConnection() {
    if (clientData.active)
        closesocket(clientData.socket);
}

SOCKET ClientConnection::getSocket() const {
    return clientData.socket;
}

std::string ClientConnection::getNickname() const {
    return clientData.nickname;
}

bool ClientConnection::isActive() const {
    return clientData.active;
}

void ClientConnection::setInactive() {
    clientData.active = false;
}

void ClientConnection::start(const std::function<void(const std::string &, const std::string &)> &commandDispatcher) {
    threadObj = std::thread(&ClientConnection::handleClient, this, commandDispatcher);
}

void ClientConnection::join() {
    if (threadObj.joinable())
        threadObj.join();
}

void
ClientConnection::handleClient(const std::function<void(const std::string &, const std::string &)> &commandDispatcher) {
    char buffer[1024];
    std::fill(&buffer[0], &buffer[1024], 0);

    while (clientData.active) {
        int bytes_received = recv(clientData.socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cout << "클라이언트 종료: " + clientData.nickname << "\n";
            clientData.active = false;
            closesocket(clientData.socket);
            break;
        }

        std::string message(buffer, bytes_received);
        trim(message);
        if (message.substr(0, 5) == "/quit") {
            std::cout << "클라이언트 종료(quit): " << clientData.nickname << "\n";
            clientData.active = false;
            closesocket(clientData.socket);
            break;
        } else if (message[0] != '/') {
            message = "/broadcast content:" + message;
        }

        commandDispatcher(message, clientData.nickname);
    }
    closesocket(clientData.socket);
}