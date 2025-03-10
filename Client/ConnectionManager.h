//
// Created by CS on 25. 3. 8.
//

#ifndef CHATPROGRAM_CONNECTIONMANAGER_H
#define CHATPROGRAM_CONNECTIONMANAGER_H

#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

// 서버와의 소켓 연결 관리 (생성, 연결, 종료)
class ConnectionManager{
public:
    ConnectionManager(std::string ip, int port);
    ~ConnectionManager();

    bool connectToServer();
    [[nodiscard]] SOCKET getSocket() const;
    void disconnect();

private:
    std::string serverIp;
    int port;
    SOCKET clientSocket;
};

#endif //CHATPROGRAM_CONNECTIONMANAGER_H
