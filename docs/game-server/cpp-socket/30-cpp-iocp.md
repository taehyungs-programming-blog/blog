---
layout: default
title: "[구현] non-block socket"
parent: "(C++) Network"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

### 블로킹 소켓

* **accept, connect, send, recv**등에 의해서 블록된다.
    * 역시 블록되는 부분이 없게 만들어 보고 싶다
* 단, 논블로킹이라해서 **성능이 좋아진다고 보장은 못한다.**
    * 매번 뺑뺑이를 돌며 논블로킹대기를 해야할까? 이건 구현에서 알아서 처리
    * 특히 논블로킹으로 구현시 코드가 복잡(더러워)진다…

---

### 블로킹(Blocking) 소켓

* **accept** -> 접속한 클라가 있을 때
* **connect** -> 서버 접속 성공했을 때
* **send, sendto** -> 요청한 데이터를 송신 버퍼에 복사했을 때
* **recv, recvfrom**-> 수신 버퍼에 도착한 데이터가 있고, 이를 유저레벨 버퍼에 복사했을 때

---

## 구현

### Server

```cpp
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << " ErrorCode : " << errCode << endl;
}

int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;
    
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        return 0;

    u_long on = 1;
    // 논블로킹(Non-Blocking)
        // ioctlsocket을 이용해서 논블로킹으로 만든다
    if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);
    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        return 0;

    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    cout << "Accept" << endl;
    SOCKADDR_IN clientAddr;
    int32 addrLen = sizeof(clientAddr);

    // Accept
    while (true)
    {
        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;   // continue로 뺑뺑이
            // Error
            break;
        }

        cout << "Client Connected!" << endl;
        // Recv
        while (true)
        {
            char recvBuffer[1000];
            int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (recvLen == SOCKET_ERROR)
            {
                // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
                if (::WSAGetLastError() == WSAEWOULDBLOCK)
                    continue;
                // Error
                break;
            }
            else if (recvLen == 0)
            {
                // 연결 끊김
                break;
            }

            cout << "Recv Data Len = " << recvLen << endl;

            // Send
            while (true)
            {
                if (::send(clientSocket, recvBuffer, recvLen, 0) == SOCKET_ERROR)
                {
                    // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
                    if (::WSAGetLastError() == WSAEWOULDBLOCK)
                        continue;
                    // Error
                    break;
                }
                cout << "Send Data ! Len = " << recvLen << endl;
                break;
            }
        }
    }
    // 윈속 종료
    ::WSACleanup();
}
```

---

### Client

```cpp
#include "pch.h"
#include <iostream>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << " ErrorCode : " << errCode << endl;
}

int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
        return 0;

    u_long on = 1;
    if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
        return 0;

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777);

    // Connect
    while (true)
    {
        if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;

            // 이미 연결된 상태라면 break
            if (::WSAGetLastError() == WSAEISCONN)
                break;

            // Error
            break;
        }
    }

    cout << "Connected to Server!" << endl;
    char sendBuffer[100] = "Hello World";

    // Send
    while (true)
    {
        if (::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
        {
            // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;
            // Error
            break;
        }

        cout << "Send Data ! Len = " << sizeof(sendBuffer) << endl;

        // Recv
        while (true)
        {
            char recvBuffer[1000];
            int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (recvLen == SOCKET_ERROR)
            {
                // 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
                if (::WSAGetLastError() == WSAEWOULDBLOCK)
                    continue;
                // Error
                break;
            }
            else if (recvLen == 0)
            {
                // 연결 끊김
                break;
            }
            cout << "Recv Data Len = " << recvLen << endl;
            break;
        }

        this_thread::sleep_for(1s);
    }
    
    // 소켓 리소스 반환
    ::closesocket(clientSocket);
    // 윈속 종료
    ::WSACleanup();
}
```