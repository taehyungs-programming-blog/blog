---
layout: default
title: "10. APC란? / Overlapped Callback 모델"
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

## 들어가기 전 APC 개념설명

* [참고사이트 🌍](https://underground2.tistory.com/150)

* **APC(Asynchronous Procedure Call)** : 
    * Thread마다 APC Queue가 존재하며 이 Queue를 APC Queue라 부른다. 
    * Thread가 APC Queue에 쌓인 함수를 처리할 준비가 됐을때(Alertable State) 
    * 해당 Thread는 APC Queue에 쌓인 모든 함수를 동작하게 된다.(자세한건 아래서 참조)

```cpp
#include <windows.h>
#include <iostream>

// APC 콜백 함수
VOID CALLBACK TimerAPCProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue) {
    std::cout << "TimerAPCProc called." << std::endl;
}

int main() {
    // 타이머 핸들
    HANDLE hTimer = NULL;

    // 타이머 생성
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (hTimer == NULL) {
        std::cerr << "CreateWaitableTimer failed." << std::endl;
        return 1;
    }

    // 5초 후에 타이머가 만료되도록 설정
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -50000000LL; // 5초

    if (!SetWaitableTimer(hTimer, &liDueTime, 0, TimerAPCProc, NULL, FALSE)) {
        std::cerr << "SetWaitableTimer failed." << std::endl;
        CloseHandle(hTimer);
        return 2;
    }

    // APC가 실행될 때까지 Alertable Wait 상태로 진입
    SleepEx(INFINITE, TRUE);

    // 핸들 정리
    CloseHandle(hTimer);
    return 0;
}
```

<br>

* 잠깐 정리한 번 해보자.

* **Overlapped 모델** (Completion Routine 콜백 기반)
    * 비동기 입출력 지원하는 소켓 생성
    * 비동기 입출력 함수 호출 (완료 루틴의 시작 주소를 넘겨준다)
    * 비동기 작업이 바로 완료되지 않으면, WSA_IO_PENDING 오류 코드
    * 비동기 입출력 함수 호출한 쓰레드를 -> Alertable Wait 상태로 만든다(Alertable Wait상태 여야지 Callback함수를 호출가능)

<br>

* Alertable Wait를 호출가능하게 만들어 주는 함수
    * --> `WaitForSingleObjectEx`, `WaitForMultipleObjectsEx`, `SleepEx`, `WSAWAitForMultipleEvents`
	* 비동기 IO 완료되면, 운영체제는 완료 루틴 호출
	* 완료 루틴 호출이 모두 끝나면, 쓰레드는 Alertable Wait 상태에서 빠져나온다

<br>

* 참고로 APC는 쓰레드마다 존재하며 Alertable Wait상태로 진입시
* APC Queue의 Callback을 처리후 다시 원래 라인으로 돌아가 일을 처리한다.
* 여기서 성능저하가 일어난다

<br>

* **Select 모델**
    * 장점) 윈도우/리눅스 공통. (클라이언트에 사용하기 좋음)
    * 단점1) 성능 최하 (매번 소켓 셋 등록 비용)
    * 단점2) 64개 제한(FD_SET 소켓 셋의 최대가 64개)
* **WSAEventSelect 모델**
    * 장점) 비교적 뛰어난 성능
    * 단점) 64개 제한
* **Overlapped (이벤트 기반)**
    * 장점) 성능
    * 단점) 64개 제한(소켓하나당 Event하나를 연결)
* **Overlapped (콜백 기반)**
    * 장점) 성능
    * 단점1) 모든 비동기 소켓 함수에서 사용 가능하진 않음 (accept).
    * 단점2) 빈번한 Alertable Wait으로 인한 성능 저하

---

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
	WSAOVERLAPPED overlapped = {};
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;	
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	cout << "Data Recv Len Callback = " << recvLen << endl;
	// TODO : 에코 서버를 만든다면 WSASend()

	Session* session = (Session*)overlapped;

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
		//WSAEVENT wsaEvent = ::WSACreateEvent();

		cout << "Client Connected !" << endl;

		while (true)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					// Pending
					// Alertable Wait					
                        // Alertable Wait APC에 있는 완료루틴을 실해해준다
					::SleepEx(INFINITE, TRUE);
					// or
					//::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE);					
				}
				else
				{
					// TODO : 문제 있는 상황
					break;
				}
			}
			else
			{
				cout << "Data Recv Len = " << recvLen << endl;
			}			
		}

		::closesocket(session.socket);
		//::WSACloseEvent(wsaEvent);
	}
	
	// 윈속 종료
	::WSACleanup();
}
```