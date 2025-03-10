//
// Created by CS on 25. 3. 4.
//

#ifndef CHATPROGRAM_CLIENT_H
#define CHATPROGRAM_CLIENT_H

#include <ws2tcpip.h>
#include <string>

// 클라이언트 정보를 저장하는 구조체
struct Client {
    SOCKET socket;
    std::string nickname;
    bool active;

    Client() : socket(-1), active(false) {}
};

#endif //CHATPROGRAM_CLIENT_H
