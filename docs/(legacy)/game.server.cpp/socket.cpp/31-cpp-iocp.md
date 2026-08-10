---
layout: default
title: "06. select model"
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

* 현재 문제가 되는 상황 👉
    * 수신버퍼에 데이터가 없는데, read 한다거나!
    * 송신버퍼가 꽉 찼는데, write 한다거나!
	* 이게 왜 문제냐? 의미없는 thread block이 발생한다
* 블로킹 소켓 : 조건이 만족되지 않아서 블로킹되는 상황 예방
* 논블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방

---

## select model

* **send, recv**를 하기 이전에 **먼저 체크**를 할 수 있다가 핵심

* 대략적 절차 정리
    * 1) 읽기소켓, 쓰기소켓, 예외(OOB)소켓 관찰 대상이 될 소켓 등록
        * (참고) 예외(OutOfBand)는 send() 마지막 인자 MSG_OOB로 보내는 특별한 데이터
        * (참고) 받는 쪽에서도 recv OOB 세팅을 해야 읽을 수 있음(잘 사용안되기에 이런게 있다고만 알자)
    * 2) `::select(readSet, writeSet, exceptSet);` -> 관찰 시작해 주세요
    * 3) 적어도 하나의 소켓이 준비되면 리턴 -> 리턴이 되었다는게 어느 하나라도 준비가 되었다는 말 AND 낙오자(fd_set)는 알아서 제거됨
    * 4) 남은 소켓 체크해서 진행

```cpp
fd_set set;

FD_ZERO(set);// 비운다

FD_SET(s, &set); // 소켓 s를 넣는다

FD_CLR(s, &set); // 소켓 s를 제거

auto ret = FD_ISSET(set); // 소켓 s가 set에 들어있으면 0이 아닌 값을 리턴한다
```

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
    // non-block으로 만든다.
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

	vector<Session> sessions;
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
        // 그냥 여기서 바로 단점 지적!!
            // (단점1) 매번 FD_ZERO, FD_SET을 해야함 

		// 소켓 셋 초기화(에코서버라서 리슨, 라이트 둘로 나뉨)
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		// ListenSocket 등록
		FD_SET(listenSocket, &reads);

		// 소켓 등록
		for (Session& s : sessions)
		{
            // (단점2) FD_SET이 Flag값을 올리는 것 이기에 64개밖에 socket을 못쓴다
			if (s.recvBytes <= s.sendBytes)
            // 받은 애가 보낼 애보다 적은 경우 더 읽어라
				FD_SET(s.socket, &reads);
			else
				FD_SET(s.socket, &writes);
		}

		// ::select가 리턴되었다는게 소켓중 어느 하나라도 준비가 되었다는 말.
            // 준비가 되었다는게 read, write buffer를 의미함 (읽을 데이터가 있거나 쓸수 있거나)
		// [옵션] 마지막 timeout 인자 설정 가능
		int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
		if (retVal == SOCKET_ERROR)
			break;

		// Listener 소켓 체크
		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}

		// 나머지 소켓 체크
		for (Session& s : sessions)
		{
			// Read
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen <= 0)
				{
					// TODO : sessions 제거
					continue;
				}

				s.recvBytes = recvLen;
			}

			// Write
			if (FD_ISSET(s.socket, &writes))
			{
				// 블로킹 모드 -> 모든 데이터 다 보냄
				// 논블로킹 모드 -> 일부만 보낼 수가 있음 (상대방 수신 버퍼 상황에 따라)
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					// TODO : sessions 제거
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}
	}

	
	// 윈속 종료
	::WSACleanup();
}
```
