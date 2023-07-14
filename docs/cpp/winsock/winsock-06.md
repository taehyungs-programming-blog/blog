---
layout: default
title: "6. IOCP"
parent: "(winsock)"
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## IOCP와 Select

* Select 모델과 IOCP (Input Output Completion Port) 모델은 둘 다 네트워크 소켓의 이벤트를 처리하는 방법을 제공하지만, 크게 다른 동작 방식을 가지고 있습니다. 이 두 모델 사이에는 여러 가지 차이점이 있으며, 이 차이점들이 왜 IOCP를 선택하는 이유가 될 수 있는지를 살펴보겠습니다.

* **확장성**: Select 모델은 효율성이 O(n)이므로, 모니터링해야 하는 소켓의 수가 증가하면 성능이 점차 떨어집니다. 이에 비해 IOCP는 대량의 동시 연결을 효과적으로 처리할 수 있는 구조로 설계되어 있습니다. 수천 개의 동시 연결을 처리하는 데 필요한 리소스는 IOCP에서는 거의 고정되어 있지만, select 모델에서는 연결 수에 비례하여 증가합니다.

* **비동기 처리**: Select 모델은 기본적으로 동기적인 동작 방식을 가지고 있습니다. 즉, **관련 이벤트가 발생할 때까지 대기**하며, 이 이벤트가 발생하면 이를 처리합니다. 이와 달리, IOCP는 비동기 I/O 작업을 지원하며, 완료 알림을 처리하기 위한 **큐 시스템**을 제공합니다. 이는 작업을 비동기적으로 처리하고 나중에 완료 알림을 받을 수 있도록 해서, 리소스 사용을 최적화하고 성능을 높이는 데 도움이 됩니다.

* **스레드 관리**: IOCP는 운영체제가 스레드 관리를 효과적으로 수행할 수 있도록 도와줍니다. 대기 중인 **I/O 작업이 있을 때만 스레드를 깨워서 CPU 사용을 최소화**하며, 스레드 풀을 사용하여 스레드 생성과 소멸에 따른 오버헤드를 줄입니다. 반면, select 모델에서는 개발자가 스레드를 직접 관리해야 할 수 있습니다.

* 따라서, 확장성, 비동기 처리 지원, 운영체제 수준에서의 효율적인 스레드 관리 등을 필요로 하는 애플리케이션에 대해서는 IOCP를 사용하는 것이 좋습니다. 이런 이유로 많은 고성능 서버 애플리케이션에서는 IOCP와 같은 비동기 I/O 모델을 사용하게 됩니다.

---

## IOCP와 Overlapped Model

* Overlapped I/O 모델과 IOCP (Input Output Completion Port) 모델은 모두 비동기 I/O 작업을 처리하는 방식입니다. 그러나 둘 사이에는 핵심적인 차이점이 있으며, 이는 왜 IOCP를 선호하는 이유를 설명합니다.

* **확장성**: Overlapped I/O는 주로 단일 스레드에서 비동기 I/O 작업을 처리하는 데 적합하며, 이는 동시에 수천 개의 연결을 처리하는 데 제한적일 수 있습니다. 반면에, IOCP는 스레드 풀과 연관되어 대량의 동시 연결을 효과적으로 처리할 수 있는 구조로 설계되어 있습니다.

* 스레드 관리: IOCP는 스레드 풀을 사용하여 스레드를 효율적으로 관리합니다. 대기 중인 I/O 작업이 있을 때만 스레드를 깨워서 CPU 사용을 최소화하며, 스레드 생성과 소멸에 따른 오버헤드를 줄입니다. Overlapped I/O에서는 이러한 기능을 별도로 구현해야 할 수 있습니다.

* 비동기 작업 관리: IOCP는 비동기 작업의 완료를 알리는 데 사용되는 큐 시스템을 제공합니다. 이는 작업 완료 알림을 효율적으로 처리하고, 애플리케이션에 필요한 정보를 제공하는 데 도움이 됩니다. Overlapped I/O에서는 각 I/O 작업의 완료를 직접 확인해야 합니다.

이러한 이유들로 인해, IOCP는 Overlapped I/O보다 더 강력하고 유연한 비동기 I/O 처리 기능을 제공합니다. 따라서 대량의 동시 연결을 처리하거나, 높은 수준의 동시성을 요구하는 고성능 애플리케이션에서는 IOCP를 사용하는 것이 보다 이상적일 수 있습니다.

* **결론**: Overlapped Model은 Thread마다 `WSARecv`를 `SleepEx`를 통해 대기하게 된다(Alertable Wait) 이게 문제임
    * 그 문제를 극복한게 **IOCP**

```cpp
// ...

typedef struct _USERSESSION
{
	SOCKET	hSocket;
	char	buffer[8192];	//8KB
} USERSESSION;

#define MAX_THREAD_CNT		4

CRITICAL_SECTION  g_cs;
std::list<SOCKET>	g_listClient;
SOCKET	g_hSocket;
HANDLE	g_hIocp;

// ...

// worker thread
DWORD WINAPI ThreadComplete(LPVOID pParam)
{
	DWORD			dwTransferredSize = 0;
	DWORD			dwFlag = 0;
	USERSESSION		*pSession = NULL;
	LPWSAOVERLAPPED	pWol = NULL;
	BOOL			bResult;

	while (1)
	{
		bResult = ::GetQueuedCompletionStatus(
			g_hIocp,				// Dequeue할 IOCP 핸들.
			&dwTransferredSize,		// 수신한 데이터 크기.
			(PULONG_PTR)&pSession,	// 수신된 데이터가 저장된 메모리
			&pWol,					// OVERLAPPED 구조체.
			INFINITE);				// 이벤트를 무한정 대기.

		if (bResult == TRUE)
		{
			// 정상

			if (dwTransferredSize == 0)
			{
                // 정상 종료
				CloseClient(pSession->hSocket);
				delete pWol;
				delete pSession;
			}
			else
			{
                // 데이터 수신
				SendMessageAll(pSession->buffer, dwTransferredSize);
				memset(pSession->buffer, 0, sizeof(pSession->buffer));

				// 다시 IOCP에 등록.
				DWORD dwReceiveSize	= 0;
				DWORD dwFlag		= 0;
				WSABUF wsaBuf		= { 0 };
				wsaBuf.buf = pSession->buffer;
				wsaBuf.len = sizeof(pSession->buffer);

				::WSARecv(
					pSession->hSocket,	//클라이언트 소켓 핸들
					&wsaBuf,			//WSABUF 구조체 배열의 주소
					1,					//배열 요소의 개수
					&dwReceiveSize,
					&dwFlag,
					pWol,
					NULL);
				if (::WSAGetLastError() != WSA_IO_PENDING)
					puts("Error IOCP Pending Queue");
			}
		}
		else
		{
			// 비정상

			if (pWol == NULL)
			{
                // iocp handle이 null(닫힘)
				puts("\tGQCS: IOCP 핸들이 닫혔습니다.");
				break;
			}
			else
			{
                // 클라이언트 비정상 종료 or 서버 연결 종료
				if (pSession != NULL)
				{
					CloseClient(pSession->hSocket);
					delete pWol;
					delete pSession;
				}
			}
		}
	}

	return 0;
}


// *** accept thread ***
DWORD WINAPI ThreadAcceptLoop(LPVOID pParam)
{
	LPWSAOVERLAPPED	pWol = NULL;
	DWORD			dwReceiveSize, dwFlag;
	USERSESSION		*pNewUser;
	int				nAddrSize = sizeof(SOCKADDR);
	WSABUF			wsaBuf;
	SOCKADDR		ClientAddr;
	SOCKET			hClient;
	int				nRecvResult = 0;

	while ((hClient = ::accept(g_hSocket,
						&ClientAddr, &nAddrSize)) != INVALID_SOCKET)
	{
		::EnterCriticalSection(&g_cs);
		g_listClient.push_back(hClient);
		::LeaveCriticalSection(&g_cs);

		pNewUser = new USERSESSION;
		::ZeroMemory(pNewUser, sizeof(USERSESSION));
		pNewUser->hSocket = hClient;    // client socket

		// iocp handle
		pWol = new WSAOVERLAPPED;
		::ZeroMemory(pWol, sizeof(WSAOVERLAPPED));

		//(연결된) 클라이언트 소켓 핸들을 IOCP에 연결.
		::CreateIoCompletionPort( (HANDLE)hClient, g_hIocp,
			(ULONG_PTR)pNewUser,		// KEY 로 사용된다
			0);

		dwReceiveSize = 0;
		dwFlag = 0;
		wsaBuf.buf = pNewUser->buffer;
		wsaBuf.len = sizeof(pNewUser->buffer);

		//클라이언트가 보낸 정보를 비동기 수신한다.
		nRecvResult = ::WSARecv(hClient, &wsaBuf, 1, &dwReceiveSize,
							&dwFlag, pWol, NULL);
		if (::WSAGetLastError() != WSA_IO_PENDING)
			puts("ERROR: WSARecv() != WSA_IO_PENDING");
	}

	return 0;
}

// *** main ***
int _tmain(int argc, _TCHAR* argv[])
{
	// init win-sock
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 0;
	}

	// init critical-section
	::InitializeCriticalSection(&g_cs);

	// Ctrl+C -> 종료 등록
	if (::SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE) == FALSE)
		printf("Ctrl+C 등록실패");

	// IOCP 생성
	g_hIocp = ::CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,	
		NULL,		        	// handle
		0,				        // Key - 해당되지 않음.
		0);				        // 스레드 개수(0은 OS가 할당)
	if (g_hIocp == NULL) {
		// fail - create iocp
		return 0;
	}

	// IOCP dispatcher thread
	HANDLE hThread;
	DWORD dwThreadID;
	for (int i = 0; i < MAX_THREAD_CNT; ++i)
	{
		dwThreadID = 0;
		hThread = ::CreateThread(NULL,
			0,
			ThreadComplete,
			(LPVOID)NULL,
			0,
			&dwThreadID);

		::CloseHandle(hThread);
	}

	// socket
	g_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
						NULL, 0, WSA_FLAG_OVERLAPPED);

	// bind/listen
	SOCKADDR_IN addrsvr;
	addrsvr.sin_family = AF_INET;
	addrsvr.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
	addrsvr.sin_port = ::htons(25000);

	if (::bind(g_hSocket,
			(SOCKADDR*)&addrsvr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		// error
		ReleaseServer();
		return 0;
	}

	if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		// error
		ReleaseServer();
		return 0;
	}

	// client accept용 thread
	hThread = ::CreateThread(NULL, 0, ThreadAcceptLoop,
					(LPVOID)NULL, 0, &dwThreadID);
	::CloseHandle(hThread);

	puts("*** server start ***");
	while (1)
		getchar();

	return 0;
}
```
