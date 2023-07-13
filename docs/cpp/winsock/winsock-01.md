---
layout: default
title: "1. WireShark 써보기 + 에코서버"
parent: "(winsock)"
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## WireShark 써보기

* WireShark 실행
* localhost(loopback) 주소를 사용할 경우 **Adapter for lookback traffic capture** 선택
* port를 아래서 사용할 예시 포트로 고정 `tcp.port == 25000`

* `Hello` 메시지를 보내본다
* WireShark로 보면 `48 65 6c 6c 6f`가 보내진것을 확인할 수 있는데 Hello이다.
    * 참고) https://onlinestringtools.com/convert-string-to-ascii

## 특정 socket이 열려있는지 확인하는 방법

```bash
$ netstat -ano | find "25000"
```

---

## 에코서버

```cpp
// Server

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	// winsock 초기화
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        // winsock Startup(Init) error
		return 0;
	}

	// socket 생성
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) {
		// create socket error
		return 0;
	}

	// port, ip bind
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR) {
		// bind error
		return 0;
	}

	// socket listen 변경
	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR) {
		// socket listen error
		return 0;
	}

	// accept client
	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = 0;
	char szBuffer[128] = { 0 };
	int nReceive = 0;

	while ((hClient = ::accept(hSocket,
		(SOCKADDR*)&clientaddr,
		&nAddrLen)) != INVALID_SOCKET) {
		puts("--- 유저접속 ---"); 
        fflush(stdout);
		
        // receive client data
		while ((nReceive = ::recv(hClient, szBuffer, sizeof(szBuffer), 0)) > 0) {
			// echo data
			::send(hClient, szBuffer, sizeof(szBuffer), 0);
			puts(szBuffer); 
            fflush(stdout);
			memset(szBuffer, 0, sizeof(szBuffer));
		}

		// close connection
		::shutdown(hSocket, SD_BOTH);
		::closesocket(hClient);
		puts("--- 유저접속종료 ---"); 
        fflush(stdout);
	}

    // cleanup
	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}
```

```cpp
// Client

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")


int _tmain(int argc, _TCHAR* argv[])
{
	// winsock 초기화
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		// winsock Startup(Init) error
		return 0;
	}

	// socket 생성
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) {
		// create socket error
		return 0;
	}

	// port, ip bind
	SOCKADDR_IN	svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (::connect(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR) {
		// error connect to server
		return 0;
	}

	// send msg
	char szBuffer[128] = { 0 };
	while (1)
	{
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0)		break;

		::send(hSocket, szBuffer, strlen(szBuffer) + 1, 0);

		memset(szBuffer, 0, sizeof(szBuffer));
		::recv(hSocket, szBuffer, sizeof(szBuffer), 0);
		printf("From server: %s\n", szBuffer);
	}

	// cleanup
	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}
```
