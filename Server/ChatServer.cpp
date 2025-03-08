//
// Created by CS on 25. 3. 5.
//
#include "ChatServer.h"
#include <iostream>


ChatServer::ChatServer(int port) : port(port), serverSocket(INVALID_SOCKET), running(false) {
    registerCommands();
}

ChatServer::~ChatServer() {
    stop();
}

bool ChatServer::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "소켓 생성 실패\n";
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "바인드 실패\n";
        return false;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "리스닝 실패\n";
        return false;
    }

    running = true;

    std::cout << "서버 시작 : " << getHostInfo() << "\n";

    acceptThread = std::thread(&ChatServer::acceptConnections, this);
    monitorThread = std::thread(&ChatServer::monitorConsole, this);
    cleaningThread = std::thread(&ChatServer::cleaningClient, this);

    return true;
}

void ChatServer::stop() {
    running = false;
    if (serverSocket != INVALID_SOCKET)
        closesocket(serverSocket);

    cv.notify_all();

    if (acceptThread.joinable()) acceptThread.join();
    if (monitorThread.joinable()) monitorThread.join();
    if (cleaningThread.joinable()) cleaningThread.join();

    {
        std::lock_guard<std::mutex> lockGuard(clientMutex);
        for (auto &client: clients) {
            if (client->isActive()) {
                shutdown(client->getSocket(), SD_BOTH);
            }
        }
    }
    std::lock_guard<std::mutex> lockGuard(clientMutex);
    for (auto &client: clients) {
        client->join();
        delete client;
    }
    clients.clear();
}

bool ChatServer::isRunning() const {
    return running;
}

void ChatServer::acceptConnections() {
    while (running) {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &addrLen);

        if (!running) {
            if (clientSocket != INVALID_SOCKET) {
                closesocket(clientSocket);
            }
            break;
        }
        if (clientSocket == -1) {
            std::cerr << "클라이언트 수락 실패\n";
            continue;
        }

        char nickBuffer[101];
        std::fill(&nickBuffer[0], &nickBuffer[101], 0);
        int bytes_received = recv(clientSocket, nickBuffer, 100, 0);
        if (bytes_received == -1) {
            closesocket(clientSocket);
            continue;
        }
        std::string nickname(nickBuffer, bytes_received);
        {
            std::lock_guard<std::mutex> lockGuard(clientMutex);
            auto *newClient = new ClientConnection(clientSocket, nickname);
            clients.push_back(newClient);
            newClient->start([this](const ChatMessage &chatMessage) {
                dispatcher.dispatch(chatMessage);
            });
        }

        std::cout << "새 클라이언트 연결 : " << nickname << " (" << inet_ntoa(clientAddr.sin_addr) << ")\n";
    }
}

void ChatServer::monitorConsole() {
    std::string input;
    while (running) {
        std::getline(std::cin, input);
        if (input == "/close") {
            std::cout << "서버 종료 명령\n";
            running = false;
            SOCKET dummy = socket(AF_INET, SOCK_STREAM, 0);
            if (dummy != -1) {
                sockaddr_in addr{};
                addr.sin_family = AF_INET;
                addr.sin_addr.s_addr = inet_addr("127.0.0.1");
                addr.sin_port = htons(port);
                connect(dummy, (sockaddr *) &addr, sizeof(addr));
                closesocket(dummy);
            }
            break;
        } else if (!input.empty()) {
            //명령어 입력
        }

    }
}

void ChatServer::cleaningClient() {
    while (running) {
        {
            std::lock_guard<std::mutex> lockGuard(clientMutex);
            clients.erase(std::remove_if(clients.begin(), clients.end(), [](ClientConnection *client) {
                return !client->isActive();
            }), clients.end());
        }
        std::unique_lock<std::mutex> uniqueLock(cvMutex);
        cv.wait_for(uniqueLock, std::chrono::minutes(1), [this] { return !running; });
    }
}

std::string ChatServer::getHostInfo() const {
    char hostBuffer[256];
    if (gethostname(hostBuffer, sizeof(hostBuffer)) == SOCKET_ERROR) {
        strncpy(hostBuffer, "unknown", sizeof(hostBuffer) - 1);
        hostBuffer[sizeof(hostBuffer) - 1] = '\0';
    }

    addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_INET;  // IPv4만 사용
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostBuffer, nullptr, &hints, &res) != 0 || res == nullptr) {
        return std::string(hostBuffer) + "(IP 조회 실패) (" + std::to_string(port) + ")";
    }

    char ipBuffer[INET_ADDRSTRLEN];
    auto *ipv4 = (sockaddr_in *) res->ai_addr;
    inet_ntop(AF_INET, &ipv4->sin_addr, ipBuffer, sizeof(ipBuffer));

    freeaddrinfo(res);

    return std::string(hostBuffer) + " - " + std::string(ipBuffer) + " (" + std::to_string(port) + ")";
}

void ChatServer::broadcastMessage(const ChatMessage &chatMessage) {
    std::string fullMessage = chatMessage.sender + " - " + chatMessage.content + '\n';
    std::cout << "브로드캐스트 : " + fullMessage;

    std::lock_guard<std::mutex> lock(clientMutex);
    for (auto client: clients) {
        send(client->getSocket(), fullMessage.c_str(), (int) fullMessage.size(), 0);
    }
}

void ChatServer::whisperMessage(const ChatMessage &chatMessage) {
    if (chatMessage.sender == chatMessage.target) return;

    std::string fullMessage = chatMessage.sender + "부터 귓속말 : " + chatMessage.content + '\n';
    std::cout << chatMessage.sender + " to " + chatMessage.target + " 귓속말 : " + fullMessage;

    bool targetFound = false;
    ClientConnection *senderClient = nullptr;

    std::lock_guard<std::mutex> lock(clientMutex);
    for (auto client: clients) {
        if (client->getNickname() == chatMessage.target && client->isActive()) {
            send(client->getSocket(), fullMessage.c_str(), (int) fullMessage.size(), 0);
            targetFound = true;
        }
        if (client->getNickname() == chatMessage.sender && client->isActive()) {
            senderClient = client;
        }
    }
    if (!targetFound && senderClient != nullptr) {

        std::string errorMessage = chatMessage.target + "은 존재하지 않거나 오프라인입니다.";
        send(senderClient->getSocket(), errorMessage.c_str(), (int) errorMessage.size(), 0);

    }
}

void ChatServer::quitMessage(const ChatMessage &chatMessage) {
    std::string fullMessage = chatMessage.sender + "이(가) 서버를 종료하였습니다.";
    std::cout << "브로드캐스트(종료메세지) : " + fullMessage;

    std::lock_guard<std::mutex> lock(clientMutex);
    for (auto client: clients) {
        send(client->getSocket(), fullMessage.c_str(), (int) fullMessage.size(), 0);
    }
}

void ChatServer::registerCommands() {
    dispatcher.registerCommand(ChatMessage::broadcast,
                               std::bind(&ChatServer::broadcastMessage, this, std::placeholders::_1));
    dispatcher.registerCommand(ChatMessage::whisper,
                               std::bind(&ChatServer::whisperMessage, this, std::placeholders::_1));
    dispatcher.registerCommand(ChatMessage::quit, std::bind(&ChatServer::quitMessage, this, std::placeholders::_1));
}
