cmake_minimum_required(VERSION 3.28)
project(ChatProgram)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(ChatProgram main.cpp
        Server/Client.h
        Server/ClientConnection.h
        Server/ClientConnection.cpp
        Server/ChatServer.h
        Server/ChatServer.cpp
        ChatMessage.h
        Server/CommandDispatcher.h
        Server/CommandDispatcher.cpp
        WinSockInitializer.h
        Client/ChatSession.h
        Client/ChatSession.cpp
        Client/ConnectionManager.h
        Client/ConnectionManager.cpp
)

target_link_libraries(ChatProgram PRIVATE ws2_32)

target_link_options(ChatProgram PRIVATE -static -static-libstdc++ -static-libgcc)
