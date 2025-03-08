#include <iostream>
#include "Server/ChatServer.h"
#include "Client/ChatSession.h"
#include "Client/ConnectionManager.h"
#include "WinSockInitializer.h"

int main() {
    WinSockInitializer wsi;
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);


    int select = 1;
    std::cout << "1. 서버 생성\n2. 클라이언트 생성\n번호를 입력하세요 : ";
    std::cin >> select;

    switch (select) {
        case 1: {

            int port;
            std::cout << "서버 포트 번호를 입력하세요: ";
            std::cin >> port;
            std::cin.ignore();

            ChatServer server(port);
            if (!server.start()) {
                std::cout << "서버 시작 실패\n";
                return -1;
            }

            while (server.isRunning()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            server.stop();

            break;
        }
        case 2: {

            std::string ip, nickname;
            int port;
            std::cin.ignore();
            std::cout << "서버 IP 주소를 입력하세요: ";
            std::getline(std::cin, ip);
            std::cout << "서버 포트 번호를 입력하세요: ";
            std::cin >> port;
            std::cin.ignore(); // 입력 버퍼 클리어
            std::cout << "닉네임을 입력하세요: ";
            std::getline(std::cin, nickname);

            ConnectionManager conn(ip, port);
            if (!conn.connectToServer()) {
                std::cerr << "서버 연결 실패\n";
                return -1;
            }

            ChatSession session(conn.getSocket(), nickname);
            session.start();

            conn.disconnect();

            break;
        }
        default:
            std::cout << "Unknown";
    }

    return 0;
}
