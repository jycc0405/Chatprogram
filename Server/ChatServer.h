//
// Created by CS on 25. 3. 5.
//

#ifndef CHATPROGRAM_CHATSERVER_H
#define CHATPROGRAM_CHATSERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

#include "Client.h"
#include "ClientConnection.h"
#include "CommandDispatcher.h"

// 서버 소켓 생성, 클라이언트 연결 수락, 메시지 처리, 콘솔 입력 모니터링 담당
class ChatServer {
public:
    explicit ChatServer(int port);

    ~ChatServer();

    bool start();

    void stop();

    bool isRunning() const;

private:
    int port;
    SOCKET serverSocket;
    bool running;
    std::vector<ClientConnection *> clients;
    std::mutex clientMutex;
    std::thread acceptThread;
    std::thread monitorThread;
    std::thread cleaningThread;
    std::mutex cvMutex;
    std::condition_variable cv;

    void acceptConnections();

    void monitorConsole();

    void cleaningClient();

    std::string getHostInfo() const;

    void broadcastMessage(const ChatMessage &chatMessage);

    void whisperMessage(const ChatMessage &chatMessage);

    void quitMessage(const ChatMessage &chatMessage);

    CommandDispatcher dispatcher;

    void registerCommands();
};

#endif //CHATPROGRAM_CHATSERVER_H
