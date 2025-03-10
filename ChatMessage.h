//
// Created by CS on 25. 3. 5.
//

#ifndef CHATPROGRAM_CHATMESSAGE_H
#define CHATPROGRAM_CHATMESSAGE_H

#include <iostream>
#include <string>
#include <utility>
#include "json/json.hpp"

// 클라이언트가 서버에게 보내는 메세지 구조체
using json = nlohmann::json;

struct ChatMessage {
    enum Command {
        quit,
        broadcast,
        whisper,
        error
    };

    Command command;
    std::string sender;
    std::string target;
    std::string content;

    // 인자 별 생성자
    ChatMessage() : command(ChatMessage::error) {}

    explicit ChatMessage(Command command)
            : command(command) {}

    ChatMessage(Command command, std::string sender)
            : command(command), sender(std::move(sender)) {}

    ChatMessage(Command command, std::string sender, std::string content)
            : command(command), sender(std::move(sender)), content(std::move(content)) {}

    ChatMessage(Command command, std::string sender, std::string target, std::string content)
            : command(command), sender(std::move(sender)), target(std::move(target)), content(std::move(content)) {}

    //구조체 내용 출력
    void showInfo() const {
        std::string commandStr;
        switch (command) {
            case quit:
                commandStr = "quit";
                break;
            case broadcast:
                commandStr = "broadcast";
                break;
            case whisper:
                commandStr = "whisper";
                break;
            case error:
                commandStr = "error";
                break;
        }
        std::cout << "Command : " << commandStr
                  << ", Sender : " << sender
                  << ", target : " << target
                  << ", content : " << content << "\n\n";
    }
};

// to_json 함수: ChatMessage -> json
inline void to_json(json &j, const ChatMessage &msg) {
    j = json{{"command", msg.command},
             {"sender",  msg.sender},
             {"target",  msg.target},
             {"content", msg.content}};
}

// from_json 함수: json -> ChatMessage
inline void from_json(const json &j, ChatMessage &msg) {
    int cmd;
    j.at("command").get_to(cmd);
    msg.command = static_cast<ChatMessage::Command>(cmd);
    j.at("sender").get_to(msg.sender);
    j.at("target").get_to(msg.target);
    j.at("content").get_to(msg.content);
}

#endif //CHATPROGRAM_CHATMESSAGE_H
