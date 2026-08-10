---
layout: default
title: "01. WinSock 만들어 보기"
parent: "(C++) Socket"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이론

* Client, Server 소켓을 만들어 볼 예정
* 우선 통신방법을 간단히 정리해 보자면.

---

* **Client**
    * 소켓 준비
    * 서버로 Connect
    * 소켓을 통해 서버와 패킷 송수신
* **Server**
    * Listener 소켓 준비
    * Bind(서버 주소/Port 소켓과 연동)
    * Listen
    * Accept
    * 소켓을 통해 Client와 통신
* 그냥 코드를 보는게 편하다 ㅎ...

---

## 구현

* 소켓프로그래밍을 하기위한 기본 include

```cpp
// ...
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
// ...
```

### Client

```cpp
int main()
{
    // (1) winsock 초기화 (ws2_32 라이브러리 초기화)
        // 관련 정보가 wsaData에 채워짐
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2),    // 2.2 버전 써달라
                    &wsaData) != 0)     // wsaData에 정보 받음
        return 0;

        // winsock을 초기화 후 ::WSACleanup(); 종료까지 불러줘야함.
        // ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
        // type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
        // protocol : 0(프로토콜은 알아서 선택해 달라)
        // return : descriptor
    SOCKET clientSocket = ::socket(AF_INET,     // IPv4
                                   SOCK_STREAM, // TCP
                                   0);          // protocol

    if (clientSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;
        return 0;
    }

    // (2) 연결할 목적지를 지정 + 연결시도
    SOCKADDR_IN serverAddr; 
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;            // IPv4
    //serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); << deprecated(구식이니 사용하지 말아주세요.)
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777);        // Port
        // htons(host to network short)란? : Endian이슈를 해결
        // Little-Endian vs Big-Endian
        // ex) 0x12345678 4바이트 정수
        // low [0x78][0x56][0x34][0x12] high <- little = PC
        // low [0x12][0x34][0x56][0x78] high <- big = network
    if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Connect ErrorCode : " << errCode << endl;
        return 0;
    }

    // ---------------------------

    // (3) 연결 성공! 이제부터 데이터 송수신 가능!
    cout << "Connected To Server!" << endl;
    while (true)
    {
        // TODO
        this_thread::sleep_for(1s);
    }

    // ---------------------------

    // (4) 종료절차
    // 소켓 리소스 반환
    ::closesocket(clientSocket);
    // 윈속 종료
    ::WSACleanup();
}
```

---

### Server

```cpp
int main()
{
    // (1) winsock 초기화  (ws2_32 라이브러리 초기화)
        // 관련 정보가 wsaData에 채워짐 (Client와 동일하다)
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;
    // ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
    // type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
    // protocol : 0
    // return : descriptor

    SOCKET listenSocket = ::socket(AF_INET,         // IPv4
                                   SOCK_STREAM,     // TCP
                                   0);
    if (listenSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket ErrorCode : " << errCode << endl;
        return 0;
    }

    // (2) 내 주소를 설정
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;                  // IPv4
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); // WinSock 니가 알아서 해줘
    serverAddr.sin_port = ::htons(7777);
    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Bind ErrorCode : " << errCode << endl;
        return 0;
    }

    // (3) 수신 시작! 
        // Client의 연결을 기다린다.
    if (::listen(listenSocket, 10) == SOCKET_ERROR) // 10 은 대기열의 의미, 테스트해서 적당한 값을 넣어야한다
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Listen ErrorCode : " << errCode << endl;
        return 0;
    }

    // -----------------------------

    while (true)
    {
        // (4) Client 정보 읽기
        SOCKADDR_IN clientAddr;
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        int32 addrLen = sizeof(clientAddr);
        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            int32 errCode = ::WSAGetLastError();
            cout << "Accept ErrorCode : " << errCode << endl;
            return 0;
        }
        // 손님 입장!
        char ipAddress[16];
        ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
        cout << "Client Connected! IP = " << ipAddress << endl;
        // TODO
    }
    
    // -----------------------------

    // (5) 종료절차
    // 윈속 종료
    ::WSACleanup();
}
```