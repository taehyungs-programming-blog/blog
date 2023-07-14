---
layout: default
title: "4. Server Multi-Thread 적용"
parent: "(winsock)"
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 현재는 Client를 하나만 받을 수 있는데 Multi-Thread를 적용해 여러 Client를 받아보자

* 결론부터 말하자면 ...
    * 이 코드가 과연 안전하다 보장할 수 있는가?
    * 최대한 Multi-Thread를 사용하고 싶지않은데? -> 다음강 참조!

```cpp
// 중요하지 않은 부분은 모두 생략함.

// ...

CRITICAL_SECTION	g_cs;
SOCKET				g_hSocket;
std::list<SOCKET>	g_listClient;

BOOL AddUser(SOCKET hSocket)
{
    // ...
}

void SendChattingMessage(char *pszParam)
{
    // ...
}

BOOL CtrlHandler(DWORD dwType)
{
	// ...
}

DWORD WINAPI ThreadFunction(LPVOID pParam)
{
	// ...
}

int _tmain(int argc, _TCHAR* argv[])
{
	//...

	while ((hClient = ::accept(g_hSocket,
		(SOCKADDR*)&clientaddr, &nAddrLen)) != INVALID_SOCKET)
	{
		if (AddUser(hClient) == FALSE)
		{
			CtrlHandler(CTRL_C_EVENT);
			break;
		}

		hThread = ::CreateThread(NULL,
			0,
			ThreadFunction,
			(LPVOID)hClient,
			0,
			&dwThreadID);

		::CloseHandle(hThread);
	}

	return 0;
}

```