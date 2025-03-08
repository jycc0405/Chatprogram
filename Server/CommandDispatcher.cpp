//
// Created by CS on 25. 3. 6.
//
#include "CommandDispatcher.h"
#include <iostream>
#include <utility>

void CommandDispatcher::registerCommand(ChatMessage::Command command, CommandHandler handler) {
    handlers[command] = std::move(handler);
}

void CommandDispatcher::dispatch(const ChatMessage &chatMessage) {
    if (chatMessage.command == ChatMessage::error) {
        std::cerr << "알 수 없는 메세지\n";
        chatMessage.showInfo();
        return;
    }

    auto it = handlers.find(chatMessage.command);
    if (it != handlers.end()) {
        it->second(chatMessage);
    } else {
        std::cerr << "알 수 없는 명령어: " << chatMessage.command << std::endl;
        chatMessage.showInfo();
    }
}
