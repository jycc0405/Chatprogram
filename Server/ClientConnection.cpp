//
// Created by CS on 25. 3. 4.
//

#include "ClientConnection.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <sstream>

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

void ClientConnection::start(const std::function<void(const ChatMessage &, SOCKET)> &broadcastFunc) {
    threadObj = std::thread(&ClientConnection::handleClient, this, broadcastFunc);
}

void ClientConnection::join() {
    if (threadObj.joinable())
        threadObj.join();
}

struct ChatMessage {
    enum class Type {
        Broadcast, Whisper
    };

    Type type;
    std::string sent;
    std::string target;
    std::string content;
};

void ClientConnection::handleClient(const std::function<void(const ChatMessage &, SOCKET)> &broadcastFunc) {
    char buffer[1024];

    while (clientData.active) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(clientData.socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cout << "클라이언트 종료: " + clientData.nickname << "\n";
            clientData.active = false;
            closesocket(clientData.socket);
            break;
        }

        std::string message(buffer, bytes_received);
        trim(message);
        if (message == "quit") {
            std::cout << "클라이언트 종료(quit): " << clientData.nickname << "\n";
            clientData.active = false;
            closesocket(clientData.socket);
            break;
        }

        std::istringstream iss(message);
        std::string type;
        ChatMessage chatMessage;

        std::getline(iss, type, ':');

        if (type == "Broadcast") {
            chatMessage.type = ChatMessage::Type::Broadcast;
            std::getline(iss, chatMessage.content);
            chatMessage.sent = clientData.nickname;
        } else if (type == "Whisper") {
            chatMessage.type = ChatMessage::Type::Whisper;
            if (!std::getline(iss, chatMessage.target, ':') || chatMessage.target.empty()) {
                std::cerr << "잘못된 귓속말 형식 (닉네임 없음): " << message << "\n";
                continue;
            }
            std::getline(iss, chatMessage.content);
            chatMessage.sent = clientData.nickname;
        } else {
            std::cerr << "타입 에러 " + clientData.nickname << "\n";
            continue;
        }

        broadcastFunc(chatMessage, clientData.socket);
    }
    closesocket(clientData.socket);
}