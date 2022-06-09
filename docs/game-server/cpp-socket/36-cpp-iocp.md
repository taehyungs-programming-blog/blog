---
layout: default
title: "[구현] Completion Port"
parent: "(C++) Socket"
grand_parent: "Game Server 👾"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이론

* **Overlapped 모델** (Completion Routine 콜백 기반)
    * 비동기 입출력 함수 완료되면, 쓰레드마다 있는 APC(Async Procedure Call) 큐에 일감이 쌓임
    * Alertable Wait 상태로 들어가서 APC 큐 비우기 (콜백 함수)
    * 단점) APC큐 쓰레드마다 있다! Alertable Wait 자체도 조금 부담!
    * 단점) 이벤트 방식 소켓:이벤트 1:1 대응

* **IOCP (Completion Port)** 모델
    * APC 대신 Completion Port 사용 (쓰레드마다 있는건 아니고 1개. 중앙에서 관리하는 APC 큐?)
    * Alertable Wait -> CP 결과 처리를 GetQueuedCompletionStatus
쓰레드랑 궁합이 아주 좋다!

* `CreateIoCompletionPort`, `GetQueuedCompletionStatus`

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

#include "Memory.h"

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
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT,
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	int32 type = 0; // read, write, accept, connect ...
};

void WorkerThreadMain(HANDLE iocpHandle)
{
	while (true)
	{
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;

		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred,
			(ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
		{
			// TODO : 연결 끊김
			continue;
		}

        // IO_TYPE::READ확인용
            // (overlappedEx->type == IO_TYPE::READ) == false일시 Crash남
		// ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);

		cout << "Recv Data IOCP = " << bytesTransferred << endl;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

        // CompletionPort에서 읽었기에 다시 WSARecv를 걸어서 다음 읽을 준비를 시킨다
		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	}
}

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
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
	
	vector<Session*> sessionManager;

	// CP 생성
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// WorkerThreads
	for (int32 i = 0; i < 5; i++)
		GThreadManager->Launch([=]() { WorkerThreadMain(iocpHandle); });

	// Main Thread = Accept 담당
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
			return 0;

		Session* session = xnew<Session>();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		cout << "Client Connected !" << endl;

		// 소켓을 CP에 등록
		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /*Key*/(ULONG_PTR)session, 0);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;

		// WSARecv를 걸어줘야 CompletionPort에서 읽어간다
		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);

		// 유저가 게임 접속 종료! 되는 현상이 발생한다면???
		//Session* s = sessionManager.back();
		//sessionManager.pop_back();
		//xdelete(s);

        // or OverlappedEx* overlappedEx = new OverlappedEx();가 delete되는 일이 있다면???
        // WSARecv를 호출하게 되면 Session, OverlappedEx등 매개변수로 들어가는 동적할당된 메모리를 해제못하게 막아야한다.
        // How?? -> 스마트포인터 레퍼런스카운팅 등
		
		//::closesocket(session.socket);
		//::WSACloseEvent(wsaEvent);
	}

	GThreadManager->Join();
	
	// 윈속 종료
	::WSACleanup();
}
```