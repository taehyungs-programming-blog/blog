---
layout: default
title: "03. UDP과 TCP"
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

* **TCP**
    * 연결을 위해 할당되는 **논리적인 경로**가 존재
    * 전송 **순서가 보장**
    * 분실이 일어나면 책임지고 다시 전송 -> **신뢰성이 좋다**
    * 고려할 것(흐름 혼잡 제어)이 많아 **속도가 느리다**
* **UDP**
    * **연결이라는 개념이 없다**
    * 전송 순서가 **보장되지 않음**
    * **경계(boundary)의 개념이 있다** 
        * -> Hello / World를 보내면 Hello / World로 들어옴(TCP는 어떻게 들어올지 모름)
    * 분실에 대한 책임이 없다 -> **신뢰성이 낮다**
    * 단순하기에 **속도가 빠르다**

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

    // UDP는 서버소켓을 하나만 만들고 그 소켓을 통해 데이터를 받음.
    SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        HandleError("Socket");
        return 0;
    }

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);

    if (::bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        HandleError("Bind");
        return 0;
    }

    while (true)
    {
        SOCKADDR_IN clientAddr;
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        int32 addrLen = sizeof(clientAddr);
        this_thread::sleep_for(1s);
        char recvBuffer[1000];

        // UDP는 Listen이 없고 바로 recv를하게 된다
        int32 recvLen = ::recvfrom(serverSocket, recvBuffer, sizeof(recvBuffer), 0,
            (SOCKADDR*)&clientAddr, &addrLen);

        if (recvLen <= 0)
        {
            HandleError("RecvFrom");
            return 0;
        }

        cout << "Recv Data! Data = " << recvBuffer << endl;
        cout << "Recv Data! Len = " << recvLen << endl;
        // 보낼때도 sendto로 보낸다
        int32 errorCode = ::sendto(serverSocket, recvBuffer, recvLen, 0,
            (SOCKADDR*)&clientAddr, sizeof(clientAddr));

        if (errorCode == SOCKET_ERROR)
        {
            HandleError("SendTo");
            return 0;
        }

        cout << "Send Data! Len = " << recvLen << endl;
    }

    // 윈속 종료
    ::WSACleanup();
}
```

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

    SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        HandleError("Socket");
        return 0;
    }

    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777);
    // Connected UDP
    ::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    // UnConnected UDP는 connect자체가 필요없음
    
    // ---------------------------
    while (true)
    {
        char sendBuffer[100] = "Hello World!";

        // 나의 IP 주소 + 포트 번호 설정
        // Unconnected UDP
        /*int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0,
            (SOCKADDR*)&serverAddr, sizeof(serverAddr));*/

        // Connected UDP
        int32 resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
        if (resultCode == SOCKET_ERROR)
        {
            HandleError("SendTo");
            return 0;
        }

        cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;
        SOCKADDR_IN recvAddr;
        ::memset(&recvAddr, 0, sizeof(recvAddr));
        int32 addrLen = sizeof(recvAddr);
        char recvBuffer[1000];

        // Unconnected UDP
        //int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0,
        //  (SOCKADDR*)&recvAddr, &addrLen);
        
        int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (recvLen <= 0)
        {
            HandleError("RecvFrom");
            return 0;
        }

        cout << "Recv Data! Data = " << recvBuffer << endl;
        cout << "Recv Data! Len = " << recvLen << endl;
        this_thread::sleep_for(1s);
    }
    
    // ---------------------------
    // 소켓 리소스 반환
    ::closesocket(clientSocket);
    // 윈속 종료
    ::WSACleanup();
}
```