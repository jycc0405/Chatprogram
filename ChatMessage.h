//
// Created by CS on 25. 3. 5.
//

#ifndef CHATPROGRAM_CHATMESSAGE_H
#define CHATPROGRAM_CHATMESSAGE_H

#include <string>
#include <sstream>
#include <unordered_map>

struct ChatMessage {
    std::string sender;
    std::string target;
    std::string content;

    ChatMessage() = default;

    explicit ChatMessage(std::string &input) {
        std::istringstream iss(input);
        std::string token;
        std::unordered_map<std::string, std::string> parsedData;

        while (iss >> token) {  // 공백을 기준으로 문자열을 분리
            if (token.find("sender:") == 0) {
                sender = token.substr(7);  // "sent:" 이후의 문자열 저장
            } else if (token.find("target:") == 0) {
                target = token.substr(7);  // "target:" 이후의 문자열 저장
            }
        }

        if (input.find("content:") == std::string::npos) {
            content = input;
        }else{
            content = input.substr(input.find("content:") + 8);
        }

    }
};

#endif //CHATPROGRAM_CHATMESSAGE_H
