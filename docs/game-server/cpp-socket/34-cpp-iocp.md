---
layout: default
title: "[구현] Overlapped Event 모델"
parent: "(C++) Socket"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Overlapped IO (비동기 + 논블로킹)

* **Overlapped IO** : 비동기(당장 동시에 실행이 되지 않아도 됨) + 논블로킹(코드를 잡지않는다)

<br>

* Overlapped 함수를 건다 (`WSARecv`, `WSASend`)
* Overlapped 함수가 성공했는지 확인 후
    * 성공했으면 결과 얻어서 처리
    * 실패했으면 사유를 확인

<br>
	
* Overlapped 모델 (이벤트 기반)
    * 비동기 입출력 지원하는 소켓 생성 + 통지 받기 위한 이벤트 객체 생성
    * 비동기 입출력 함수 호출 (1에서 만든 이벤트 객체를 같이 넘겨줌)
    * 비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드
    * 운영체제는 이벤트 객체를 signaled 상태로 만들어서 완료 상태 알려줌
        * `WSAWaitForMultipleEvents` 함수 호출해서 이벤트 객체의 signal 판별
        * `WSAGetOverlappedResult` 호출해서 비동기 입출력 결과 확인 및 데이터 처리

<br>

* 음? `WSAEventSelect`도 Event를 통해 Signal을 받는데. 뭐가 다른가?
    * **Overlapped** 로 동작하는게 핵심이다. 비동기 + 논블로킹
    * 아래에 `WSAWaitForMultipleEvents`이 있어서 Pending되는거 같이 보이나 `WSAWaitForMultipleEvents`를 호출하지 않는다면 비동기 + 논블로킹으로 동작하게 된다.

---

## Server

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

const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	WSAOVERLAPPED overlapped = {};
};

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
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

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket;
		while (true)
		{
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;

			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			// 문제 있는 상황
			return 0;
		}

		Session session = Session{ clientSocket };
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << "Client Connected !" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			// WSARecv를 통해 받고 문제가 없다면(!= SOCKET_ERROR)
			// sendLen를 통해 얼마나 보냈는지 flag가 뭔지 알려줌
			// WSARecv를 통해 받고 문제가 있다면(== SOCKET_ERROR)
			// ::WSAGetOverlappedResult를 통해 결과를 알려줌
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, nullptr) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Pending
					::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
					::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
				}
				else
				{
					// TODO : 문제 있는 상황
					break;
				}
			}

			cout << "Data Recv Len = " << recvLen << endl;
		}

		::closesocket(session.socket);
		::WSACloseEvent(wsaEvent);
	}
	
	// 윈속 종료
	::WSACleanup();
}
```

---

## Client

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
	WSAEVENT wsaEvent = ::WSACreateEvent();
	WSAOVERLAPPED overlapped = {};
	overlapped.hEvent = wsaEvent;

	// Send
	while (true)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = sendBuffer;
		wsaBuf.len = 100;

		DWORD sendLen = 0;
		DWORD flags = 0;
		// WSASend를 통해 보내고 문제가 없다면(!= SOCKET_ERROR)
			// sendLen를 통해 얼마나 보냈는지 flag가 뭔지 알려줌
		// WSASend를 통해 보내고 문제가 있다면(== SOCKET_ERROR)
			// ::WSAGetOverlappedResult를 통해 결과를 알려줌
		if (::WSASend(clientSocket, &wsaBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				// Pending
				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
				::WSAGetOverlappedResult(clientSocket, &overlapped, &sendLen, FALSE, &flags);
			}
			else
			{
				// 진짜 문제 있는 상황
				break;
			}
		}

		cout << "Send Data ! Len = " << sizeof(sendBuffer) << endl;

		this_thread::sleep_for(1s);
	}

	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 윈속 종료
	::WSACleanup();
}
```