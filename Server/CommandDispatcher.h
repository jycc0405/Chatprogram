//
// Created by CS on 25. 3. 6.
//

#ifndef CHATPROGRAM_COMMANDDISPATCHER_H
#define CHATPROGRAM_COMMANDDISPATCHER_H

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

#include "../ChatMessage.h"

// 각 ChatMessage의 command에 따라 함수를 등록하고 handler를 호출하여 처리
using CommandHandler = std::function<void(const ChatMessage &chatMessage)>;

class CommandDispatcher {
public:
    void registerCommand(ChatMessage::Command command, CommandHandler handler);

    void dispatch(const ChatMessage & chatMessage);

private:
    std::unordered_map<ChatMessage::Command, CommandHandler> handlers;
};

#endif //CHATPROGRAM_COMMANDDISPATCHER_H
