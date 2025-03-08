//
// Created by CS on 25. 3. 8.
//
#include "ChatSession.h"
#include <iostream>
#include <sstream>
#include <utility>

ChatSession::ChatSession(SOCKET socket, std::string nickname)
        : socket(socket), nickname(std::move(nickname)), active(true) {

}

ChatSession::~ChatSession() {
    if (socket != INVALID_SOCKET) {
        closesocket(socket);
    }
}

void ChatSession::start() {
    if (send(socket, nickname.c_str(), (int) nickname.size(), 0) == INVALID_SOCKET) {
        std::cerr << "서버 연결 실패\n";
        return;
    }
    std::cout << "서버 연결 성공. 채팅을 시작하세요. 종료는 '/quit' 입력하세요.\n";

    recvThread = std::thread(&ChatSession::receiveMessage, this);
    sendMessage();
    if (recvThread.joinable())
        recvThread.join();

}

void ChatSession::sendMessage() {
    std::string message;
    while (active) {
        std::getline(std::cin, message);
        if (message.empty())
            continue;
        else {
            ChatMessage chatMessage = transformInputParser(message);
            json j = chatMessage;
            std::string serialized = j.dump();
            send(socket, serialized.c_str(), (int) serialized.size(), 0);

            if (message == "/quit") {
                shutdown(socket, SD_BOTH);  // 소켓의 송수신을 종료하여 recv가 0을 반환하도록 함
                active = false;
                std::cout << "연결 종료. 재접속을 위해 프로그램을 재시작하세요.\n";
                break;
            }
        }
    }
}

void ChatSession::receiveMessage() {
    char buffer[1024];
    std::fill(&buffer[0], &buffer[1024], 0);
    while (active) {
        int bytes_received = recv(socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            if (!active)
                break;
            std::cerr << "서버로부터의 연결 종료 또는 에러\n";
            active = false;
            break;
        }
        std::string receivedMsg(buffer, bytes_received);
        std::cout << receivedMsg << "\n";
    }
}

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(' ');
    return (start == std::string::npos) ? "" : s.substr(start);
}

ChatMessage ChatSession::transformInputParser(const std::string &input) {
    if (input[0] != '/') {
        return {ChatMessage::broadcast, nickname, input};
    }

    // '/'를 제거한 후 스트림 생성
    std::istringstream iss(input.substr(1));
    std::string command;
    iss >> command;  // 첫 번째 토큰: 명령어

    // 나머지 전체 문자열을 읽고 왼쪽 공백 제거
    std::string remainder;
    std::getline(iss, remainder);
    remainder = ltrim(remainder);

    ChatMessage chatMessage;

    // 명령어에 따른 분기
    if (command == "broadcast") {
        return {ChatMessage::broadcast, nickname, remainder};

    } else if (command == "whisper" || command == "w") {
        std::istringstream restStream(remainder);

        std::string target;
        restStream >> target;  // 두 번째 토큰: 대상
        if (target.empty()) {
            std::cout << "귓속말 대상을 입력해주세요.\n";
            return {ChatMessage::error,nickname};
        }

        std::string content;
        std::getline(restStream, content);
        content = ltrim(content);
        if (content.empty()) {
            std::cout << "귓속말 내용을 입력해주세요.\n";
            return {ChatMessage::error,nickname};
        }

        return {ChatMessage::whisper, nickname, target, content};

    } else if (command == "quit") {
        return {ChatMessage::quit,nickname};

    } else {
        // 인식되지 않는 명령어의 경우 기본 broadcast로 처리
        return {ChatMessage::broadcast, nickname, input};
    }
}