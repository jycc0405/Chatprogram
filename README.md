# C++ 기반 채팅 프로그램

#### 이 프로젝트는 C++로 작성된 간단한 클라이언트-서버 구조의 채팅 프로그램입니다. 이 프로그램은 소켓 통신을 사용하여 여러 클라이언트가 하나의 서버에 접속하여 서로 메시지를 주고받을 수 있도록 구현되었습니다.

## 주요 기능

- **클라이언트-서버 아키텍처**: 서버는 다수의 클라이언트 연결을 처리하고, 클라이언트는 서버에 접속하여 채팅에 참여합니다.

- **다중 스레드 기반 서버**: 서버는 새로운 클라이언트 연결을 수락하고, 각 클라이언트의 메시지 처리를 위한 전용 스레드를 생성하여 동시성을 확보합니다.

- **메시지 전송**: 전체 사용자에게 메시지를 보내는 **브로드캐스트(broadcast)** 와 특정 사용자에게만 메시지를 보내는 **귓속말(whisper)** 기능을 지원합니다.

- **JSON을 이용한 데이터 직렬화**: 메시지 데이터는 nlohmann/json 라이브러리를 사용하여 JSON 형식으로 직렬화 및 역직렬화되어 전송됩니다.


---

## 프로젝트 구조

프로젝트는 크게 **클라이언트(Client)** 와 **서버(Server)** 두 부분으로 나뉘며, 메시지 데이터 구조와 같은 공통 로직은 별도의 파일에 정의되어 있습니다.

- `main.cpp`: 프로그램의 진입점입니다. 사용자의 선택에 따라 서버 또는 클라이언트를 시작합니다.

- `Client`: 클라이언트 로직

  - `ChatSession.cpp`: 사용자 입력을 처리하고, 서버로부터 메시지를 수신하는 핵심 클라이언트 로직을 담당합니다.

  - `ConnectionManager.cpp`: 서버와의 연결을 관리합니다.

- `Server`: 서버 로직

  - `ChatServer`: 클라이언트 연결 수락 및 관리를 담당하는 메인 서버 클래스입니다.
 
  - `Client`: 클라이언트 정보를 저장하는 구조체입니다.
    
  - `ClientConnection.cpp`: 각 클라이언트와의 통신을 독립적으로 처리합니다.
 
  - `CommandDispatcher.cpp`: 메시지 명령어에 따라 적절한 핸들러 함수를 호출하는 디스패처 패턴을 구현했습니다.

- `공통`: 클라이언트와 서버가 공유하는 로직입니다.

  - `ChatMessage.h`: 채팅 메시지 데이터를 정의하는 구조체입니다.

 ---

## 핵심 로직 상세 설명

### CommandDispatcher 클래스

이 프로젝트의 핵심 패턴 중 하나인 CommandDispatcher 클래스는 `CommandDispatcher.cpp`와 `ChatServer.cpp` 파일에서 구현됩니다. 이 클래스는 들어오는 메시지의 `command`에 따라 특정 기능을 수행하는 핸들러 함수를 연결하고 호출하는 역할을 합니다.

코드 예시 (`ChatServer.cpp`의 `registerCommands` 메서드)

```C++

void ChatServer::registerCommands() {
    // broadcast 명령어가 들어오면 broadcastMessage 함수 호출
    dispatcher.registerCommand(ChatMessage::broadcast,
                               std::bind(&ChatServer::broadcastMessage, this, std::placeholders::_1));

    // whisper 명령어가 들어오면 whisperMessage 함수 호출
    dispatcher.registerCommand(ChatMessage::whisper,
                               std::bind(&ChatServer::whisperMessage, this, std::placeholders::_1));

    // quit 명령어가 들어오면 quitMessage 함수 호출
    dispatcher.registerCommand(ChatMessage::quit, std::bind(&ChatServer::quitMessage, this, std::placeholders::_1));
}
```

### 예시 코드의 강점

1. **높은 가독성 및 유지보수성**: registerCommands 함수를 보면 어떤 명령어가 어떤 함수에 연결되는지 명확하게 알 수 있습니다. 새로운 명령어를 추가하거나 기존 명령어를 수정할 때, 이 함수에 한 줄만 추가하거나 수정하면 되므로 코드의 확장과 유지보수가 용이합니다.

2. **낮은 결합도**: CommandDispatcher는 특정 메시지 핸들러 함수를 직접 호출하는 대신, 함수 포인터(std::function)를 통해 간접적으로 호출합니다. 이로 인해 CommandDispatcher는 ChatServer 클래스의 구체적인 구현 내용(예: broadcastMessage 함수)에 종속되지 않고, 재사용 가능한 유연한 구조를 가집니다.

3. **명령어와 기능의 분리**: ChatServer는 메시지 처리 로직을 직접 dispatch 함수 내부에 구현하는 대신, 각 기능별로 분리된 함수(broadcastMessage, whisperMessage 등)를 정의하고 이를 CommandDispatcher에 등록하는 방식을 사용합니다. 이는 **단일 책임 원칙(Single Responsibility Principle)**을 따르는 좋은 방법입니다.
