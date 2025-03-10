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
    // 서버에 닉네임 전송
    if (send(socket, nickname.c_str(), (int) nickname.size(), 0) == INVALID_SOCKET) {
        std::cerr << "서버 연결 실패\n";
        return;
    }
    std::cout << "서버 연결 성공. 채팅을 시작하세요. 종료는 '/quit' 입력하세요.\n";

    // 메세지 수신 쓰레드 시작
    recvThread = std::thread(&ChatSession::receiveMessage, this);
    // 메세지 송신 쓰레드는 메인 쓰레드에서 시작
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
            // 입력을 ChatMessage구조체에 저장 후 Json으로 전송
            ChatMessage chatMessage = transformInputParser(message);
            json j = chatMessage;
            std::string serialized = j.dump();
            send(socket, serialized.c_str(), (int) serialized.size(), 0);

            // "/quit" 입력 시 클라이언트 종료
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
        // 서버로부터 가공된 메세지만 수신(메세지 처리 불필요)
        std::cout << receivedMsg << "\n";
    }
}

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(' ');
    return (start == std::string::npos) ? "" : s.substr(start);
}

ChatMessage ChatSession::transformInputParser(const std::string &input) {
    // 첫 글자가 '/'가 아니라면 "broadcast"로 전송
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
        std::getline(restStream, content); // 세 번째 토큰: 내용
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