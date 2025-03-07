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

using CommandHandler = std::function<void(const ChatMessage &chatMessage)>;

class CommandDispatcher {
public:
    void registerCommand(const std::string &command, CommandHandler handler);

    void dispatch(const std::string &input,const std::string &sender);

private:
    std::unordered_map<std::string, CommandHandler> handlers;
};

#endif //CHATPROGRAM_COMMANDDISPATCHER_H
