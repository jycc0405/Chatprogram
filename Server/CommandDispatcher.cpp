//
// Created by CS on 25. 3. 6.
//
#include "CommandDispatcher.h"
#include <iostream>
#include <sstream>

void CommandDispatcher::registerCommand(const std::string &command, CommandHandler handler) {
    handlers[command] = handler;
}

void CommandDispatcher::dispatch(const std::string &input, const std::string &sender) {
    if (input.empty() || input[0] != '/') {
        std::cerr << "알 수 없는 메세지" << input << "\n";
        return;
    }
    std::istringstream iss(input);
    std::string command;

    iss >> command;
    if (!command.empty() && command[0] == '/') {
        command = command.substr(1);
    }

    std::string message;
    iss>>message;
    message="sender:"+sender+" "+message;
    ChatMessage chatMessage(message);

    auto it = handlers.find(command);
    if (it != handlers.end()) {
        it->second(chatMessage);
    } else {
        std::cerr << "알 수 없는 명령어: " << command << std::endl;
    }
}
