//
// Created by CS on 25. 3. 4.
//

#include "ClientConnection.h"
#include <iostream>
#include <algorithm>
#include <cctype>

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

void ClientConnection::start(const std::function<void(const ChatMessage &)> &commandDispatcher) {
    threadObj = std::thread(&ClientConnection::handleClient, this, commandDispatcher);
}

void ClientConnection::join() {
    if (threadObj.joinable())
        threadObj.join();
}

void
ClientConnection::handleClient(const std::function<void(const ChatMessage &)> &commandDispatcher) {
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

        std::string jsonString(buffer, bytes_received);
        try {
            json j = json::parse(jsonString);
            ChatMessage chatMessage = j.get<ChatMessage>();

            chatMessage.sender = getNickname();

            if (chatMessage.command == ChatMessage::quit) {
                std::cout << "클라이언트 종료(quit): " << clientData.nickname << "\n";
                clientData.active = false;
                closesocket(clientData.socket);
                commandDispatcher(chatMessage);
                break;
            }

            if (chatMessage.command != ChatMessage::error)
                commandDispatcher(chatMessage);

        } catch (json::parse_error &e) {
            std::cerr << "JSON 파싱 실패: " << e.what() << std::endl;
        }
    }
    closesocket(clientData.socket);
}