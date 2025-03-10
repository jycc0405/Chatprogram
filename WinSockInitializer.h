//
// Created by CS on 25. 3. 6.
//

#ifndef CHATPROGRAM_WINSOCKINITIALIZER_H
#define CHATPROGRAM_WINSOCKINITIALIZER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

// Windows용 Winsock 초기화 클래스
class WinSockInitializer{
public:
    WinSockInitializer(){
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
            std::cerr << "WSAStartup 실패\n";
        }
    }

    ~WinSockInitializer(){
        WSACleanup();
    }
};

#endif //CHATPROGRAM_WINSOCKINITIALIZER_H
