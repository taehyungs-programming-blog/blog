---
layout: default
title: "5. I/O 멀티플렉싱 - thread쓰지 않고 동시입력 처리하기"
parent: "(winsock)"
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## select

* 우선 아래 예시는 select를 사용한 예시이다.
* 중점적으로 볼 부분은 thread를 쓰지 않았음에 유의해서 보자
* 역시 결론부터 말하자면 ....
    * 요즘은 select를 쓰지 않는다
    * select는 오직 64개의 I/O Event만 처리가능하기 때문이다!
    * 여기서 확인할 사항은 thread를 쓰지 않고 이런 처리가 가능하다 정도이다

```cpp
// ...

SOCKET				g_hSocket;
std::list<SOCKET>	g_listClient;

void SendMessageAll(char *pszMessage, int nSize)
{
    // ...
}

void CloseAll(void)
{
	// ...
}

BOOL CtrlHandler(DWORD dwType)
{
	// ...
}

int _tmain(int argc, _TCHAR* argv[])
{
	// ...

	UINT nCount;
	FD_SET fdRead;
	std::list<SOCKET>::iterator it;

	do
	{
		// select로 감지 시작
		FD_ZERO(&fdRead);
		for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
			FD_SET(*it, &fdRead);

		// 감지 대기
		::select(0, &fdRead, NULL, NULL, NULL);

		// 감지가 확인되면
		nCount = fdRead.fd_count;
		for (int nIndex = 0; nIndex < nCount; ++nIndex)
		{
			if (!FD_ISSET(fdRead.fd_array[nIndex], &fdRead))
				continue;

			if (fdRead.fd_array[nIndex] == g_hSocket)
			{
				SOCKADDR_IN clientaddr = { 0 };
				int nAddrLen = sizeof(clientaddr);
				SOCKET hClient = ::accept(g_hSocket,
									(SOCKADDR*)&clientaddr, &nAddrLen);
				if (hClient != INVALID_SOCKET)
				{
					FD_SET(hClient, &fdRead);
					g_listClient.push_back(hClient);
				}
			}
			else
			{
				char szBuffer[1024] = { 0 };
				int nReceive = ::recv(fdRead.fd_array[nIndex],
								(char*)szBuffer, sizeof(szBuffer), 0);
				if (nReceive <= 0)
				{
					//연결 종료.
					::closesocket(fdRead.fd_array[nIndex]);
					FD_CLR(fdRead.fd_array[nIndex], &fdRead);
					g_listClient.remove(fdRead.fd_array[nIndex]);
				}
				else
				{
					SendMessageAll(szBuffer, nReceive);
				}
			}
		} 
	} while (g_hSocket != NULL);

	CloseAll();
	return 0;
}
```

---

## event select

* 역시 이것도 이런방식이 있다. 정도로 이해하자
    * 역시 최대 64개만 지원하기에 이것도 못쓰인다 ...

```cpp
int _tmain(int argc, _TCHAR* argv[])
{
	// ...

	g_nListIndex = 0;
	g_aListSocket[g_nListIndex] = g_hSocket;
	g_aListEvent[g_nListIndex] = ::WSACreateEvent();
	
	if (::WSAEventSelect(g_hSocket, g_aListEvent[g_nListIndex],
			FD_ACCEPT) == SOCKET_ERROR)
	{
		return 0;
	}

	DWORD dwIndex;
	WSANETWORKEVENTS netEvent;
	while (TRUE)
	{
		// 이벤트 대기
		dwIndex = ::WSAWaitForMultipleEvents(
						g_nListIndex + 1,
						g_aListEvent,
						FALSE,
						100,
						FALSE);
		if (dwIndex == WSA_WAIT_FAILED)
			continue;

		if (::WSAEnumNetworkEvents(g_aListSocket[dwIndex],
				g_aListEvent[dwIndex], &netEvent) == SOCKET_ERROR)
			continue;


		if (netEvent.lNetworkEvents & FD_ACCEPT)
		{
			if (netEvent.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			if (g_nListIndex >= WSA_MAXIMUM_WAIT_EVENTS)
			{
				continue;
			}

			SOCKADDR_IN clientaddr = { 0 };
			int nAddrLen = sizeof(clientaddr);
			SOCKET hClient = ::accept(g_hSocket,
								(SOCKADDR*)&clientaddr, &nAddrLen);
			if (hClient != INVALID_SOCKET)
			{
                // 새 클라이언트 연결
				++g_nListIndex;
				g_aListSocket[g_nListIndex] = hClient;
				g_aListEvent[g_nListIndex] = ::WSACreateEvent();
			}

			::WSAEventSelect(hClient, g_aListEvent[g_nListIndex],
				FD_READ | FD_CLOSE);
		}
		else if (netEvent.lNetworkEvents & FD_CLOSE)
		{
            // 클라이언트 연결 종료
			::WSACloseEvent(g_aListEvent[dwIndex]);
			::shutdown(g_aListSocket[dwIndex], SD_BOTH);
			::closesocket(g_aListSocket[dwIndex]);

			for (DWORD i = dwIndex; i < g_nListIndex; ++i)
			{
				g_aListEvent[i] = g_aListEvent[i + 1];
				g_aListSocket[i] = g_aListSocket[i + 1];
			}

			g_nListIndex--;
		}
		else if (netEvent.lNetworkEvents & FD_READ)
		{
            // 데이터 
			TCHAR szBuffer[1024] = { 0 };
			int nRecv = ::recv(g_aListSocket[dwIndex],
				(char*)szBuffer, sizeof(szBuffer), 0);

			for (DWORD i = 1; i < g_nListIndex + 1; ++i)
				::send(g_aListSocket[i], (char*)szBuffer, nRecv, 0);
		}
	}

	return 0;
}
```