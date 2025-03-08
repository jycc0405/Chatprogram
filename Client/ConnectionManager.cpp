//
// Created by CS on 25. 3. 8.
//
#include "ConnectionManager.h"

#include <iostream>
#include <utility>

ConnectionManager::ConnectionManager(std::string ip, int port)
        : serverIp(std::move(ip)), port(port), clientSocket(-1) {}

ConnectionManager::~ConnectionManager() {
    disconnect();
}

bool ConnectionManager::connectToServer() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "소켓 생성 실패\n";
        return false;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIp.c_str(), &server_addr.sin_addr) <= 0) {
        std::cerr << "잘못된 IP 주소\n";
        return false;
    }
    if (connect(clientSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "서버 연결 실패\n";
        return false;
    }

    return true;
}

SOCKET ConnectionManager::getSocket() const {
    return clientSocket;
}

void ConnectionManager::disconnect() {
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
        clientSocket = -1;
    }
}