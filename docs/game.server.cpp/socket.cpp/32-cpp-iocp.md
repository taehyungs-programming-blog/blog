---
layout: default
title: "07. WSAEventSelect model"
parent: "(C++) Socket"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Select 방식의 문제점

* 매번 소켓 셋을 초기화 하고 socket에 등록을 해야한다.

```cpp
// 아래부분

FD_ZERO(&reads);
FD_ZERO(&writes);

FD_SET(listenSocket, &reads)

for(Session& s : sessions)
{
    // ...
}
```

---

## WSAEventSelect 모델

* `WSAEventSelect` = (WSAEventSelect 함수가 핵심이 되는)
	* ISSET을 WindowEvent로 체크하는것이다.
* (핵심) 소켓과 관련된 네트워크 이벤트를 [이벤트 객체]를 통해 감지

<br>

* 이벤트 객체 관련 함수들
    * 생성 : `WSACreateEvent` (수동 리셋 Manual-Reset + Non-Signaled 상태 시작)
    * 삭제 : `WSACloseEvent`
    * 신호 상태 감지 : `WSAWaitForMultipleEvents`
    * 구체적인 네트워크 이벤트 알아내기 : `WSAEnumNetworkEvents`

<br>

* `WSAEventSelect(socket, event, networkEvents);`
    * 관심있는 네트워크 이벤트, 아래 이벤트가 오면 Signal을 주세요
    * **FD_ACCEPT** : 접속한 클라가 있음 accept
    * **FD_READ** : 데이터 수신 가능 recv, recvfrom
    * **FD_WRITE** : 데이터 송신 가능 send, sendto
    * **FD_CLOSE** : 상대가 접속 종료
    * **FD_CONNECT** : 통신을 위한 연결 절차 완료
    * **FD_OOB**

<br>

* 주의 사항
    * `WSAEventSelect` 함수를 호출하면, 해당 소켓은 **자동으로 non-block** 모드 전환
    * `accept()` 함수가 리턴하는 소켓은 listenSocket과 동일한 속성을 갖는다
        * 따라서 clientSocket은 FD_READ, FD_WRITE 등을 다시 등록 필요
        * 드물게 WSAEWOULDBLOCK 오류가 뜰 수 있으니 예외 처리 필요
* 중요
    * 이벤트 발생 시, 적절한 소켓 함수 호출해야 함
    * 아니면 다음 번에는 동일 네트워크 이벤트가 발생 X
        * ex) FD_READ 이벤트 떴으면 recv() 호출해야 하고, 안하면 FD_READ 두 번 다시 X

---

## 구현

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
	int32 sendBytes = 0;
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

	vector<WSAEVENT> wsaEvents;
	vector<Session> sessions;
	sessions.reserve(100);

	WSAEVENT listenEvent = ::WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sessions.push_back(Session{ listenSocket });
	// 소켓(listenSocket)과 이벤트(listenEvent)를 연결한다.
	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
		return 0;

	while (true)
	{
		int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED)
			continue;

		index -= WSA_WAIT_EVENT_0;

		//::WSAResetEvent(wsaEvents[index]);

		WSANETWORKEVENTS networkEvents;
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
			continue;

		// Listener 소켓 체크
		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			// Error-Check
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);

			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected" << endl;

				WSAEVENT clientEvent = ::WSACreateEvent();
				wsaEvents.push_back(clientEvent);
				sessions.push_back(Session{ clientSocket });
				if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
					return 0;
			}
		}

		// Client Session 소켓 체크
		if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
		{
			// Error-Check
			if ((networkEvents.lNetworkEvents & FD_READ) && (networkEvents.iErrorCode[FD_READ_BIT] != 0))
				continue;
			// Error-Check
			if ((networkEvents.lNetworkEvents & FD_WRITE) && (networkEvents.iErrorCode[FD_WRITE_BIT] != 0))
				continue;

			Session& s = sessions[index];

			// Read
			if (s.recvBytes == 0)
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// TODO : Remove Session
					continue;
				}

				s.recvBytes = recvLen;
				cout << "Recv Data = " << recvLen << endl;
			}

			// Write
			if (s.recvBytes > s.sendBytes)
			{
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// TODO : Remove Session
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}

				cout << "Send Data = " << sendLen << endl;
			}
		}

		// FD_CLOSE 처리
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			// TODO : Remove Socket
		}
	}

	
	// 윈속 종료
	::WSACleanup();
}
```