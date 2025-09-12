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
 
  - `WinSockInitializer`: Winsock 초기화 클래스입니다.

 ---

## 핵심 로직 상세 설명

### ChatServer 클래스의 ServerStart 메서드

이 메서드는 **채팅 서버를 초기화하고 시작하는 핵심 로직**을 담당합니다. 서버 소켓을 생성하고, 특정 주소와 포트에 바인딩하며, 클라이언트 연결을 수락할 준비를 마친 후, 동시에 여러 작업을 처리하기 위해 멀티 스레드를 시작합니다.

코드 예시 (`ChatServer.cpp`의 `start` 메서드)

```C++

bool ChatServer::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "소켓 생성 실패\n";
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "바인드 실패\n";
        return false;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "리스닝 실패\n";
        return false;
    }

    running = true;

    std::cout << "서버 시작 : " << getHostInfo() << "\n";

    acceptThread = std::thread(&ChatServer::acceptConnections, this); // 연결 수락 쓰레드
    monitorThread = std::thread(&ChatServer::monitorConsole, this); // 콘솔 처리 쓰레드
    cleaningThread = std::thread(&ChatServer::cleaningClient, this); // 종료된 클라이언트 정리 쓰레드

    return true;
}
```

#### 코드 설명

- serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  - 소켓 생성: AF_INET (IPv4) 주소 체계를 사용하고 SOCK_STREAM (TCP) 타입의 소켓을 생성합니다. 이 소켓이 클라이언트 연결을 수락하는 서버의 문 역할을 합니다.

- bind(serverSocket, ...);

  - bind: 생성된 소켓을 위에서 설정한 주소(serverAddr)에 연결합니다.

- listen(serverSocket, 5);

  - 연결 대기: 서버 소켓을 클라이언트의 접속 요청을 들을 수 있는 "리스닝" 상태로 전환합니다. 5는 동시에 대기열에 올릴 수 있는 최대 연결 요청 수를 의미합니다.

- 멀티 스레드 시작

  - 서버는 클라이언트 연결을 동시에 처리하기 위해 여러 스레드를 생성합니다.

  - `acceptThread`: `acceptConnections` 메서드를 실행하여 클라이언트의 연결 요청을 지속적으로 수락합니다.

  - `monitorThread`: `monitorConsole` 메서드를 실행하여 사용자가 콘솔에 입력하는 명령어를 처리합니다.

  - `cleaningThread`: `cleaningClient` 메서드를 실행하여 연결이 종료된 클라이언트 소켓을 정리하는 작업을 수행합니다.

  
start() 메서드는 이 모든 초기화 과정이 성공하면 true를 반환하며, 서버가 정상적으로 작동을 시작했음을 알립니다. 만약 소켓 생성, 바인딩, 리스닝 과정 중 하나라도 실패하면 오류 메시지를 출력하고 false를 반환합니다.

### 예시 코드의 강점

1. **견고한 오류 처리 및 방어적 프로그래밍**: 서버 소켓 생성, 바인딩, 리스닝 등 각 단계에서 발생할 수 있는 오류를 명시적으로 확인하고 실패 시 즉시 종료합니다. 이는 예외 상황으로 인한 프로그램의 비정상적인 동작을 방지하고, 서버가 항상 신뢰할 수 있는 상태에서 시작되도록 보장합니다.

2. **효율적인 멀티태스킹(Multi-tasking)을 위한 스레드 분리**: 클라이언트 연결 수락, 콘솔 명령어 모니터링, 종료된 클라이언트 정리와 같은 핵심 기능을 각각 별도의 스레드로 분리했습니다. 이로 인해 서버는 하나의 작업이 다른 작업의 진행을 막지 않고, 동시에 여러 클라이언트를 처리할 수 있는 **동시성(Concurrency)** 을 확보하여 성능과 응답성을 크게 향상시킵니다.

3. **명확한 역할 분리 및 모듈화**: start() 메서드는 서버 초기화 및 스레드 시작이라는 단일한 역할에만 집중합니다. 각 스레드가 담당하는 실제 로직은 acceptConnections, monitorConsole, cleaningClient와 같은 별도의 메서드에 구현되어 있어, 코드의 가독성을 높이고 유지보수를 용이하게 만듭니다.
