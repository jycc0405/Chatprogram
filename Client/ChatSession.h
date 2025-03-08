//
// Created by CS on 25. 3. 8.
//

#ifndef CHATPROGRAM_CHATSESSION_H
#define CHATPROGRAM_CHATSESSION_H

#include <string>
#include <thread>
#include "../ChatMessage.h"

#include <winsock2.h>
#include <ws2tcpip.h>

class ChatSession{
public:
    ChatSession(SOCKET socket, std::string nickname);
    ~ChatSession();

    void start();

private:
    SOCKET socket;
    std::string nickname;
    bool active;
    std::thread recvThread;

    void sendMessage();
    void receiveMessage();
    ChatMessage transformInputParser(const std::string &input);
};

#endif //CHATPROGRAM_CHATSESSION_H
