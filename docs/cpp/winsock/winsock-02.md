---
layout: default
title: "2. Nagle 알고리즘"
parent: "(winsock)"
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* **Nagle Algorithm** - 매번 send/recv를 할 경우 오버헤드가 발생하니 일정 buffer에 모았다 send/recv해보자.
* 우선 입/출력 버퍼의 사이즈를 확인하자

## 버퍼 사이즈 측정

```cpp
#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 0;
	}

	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) {
		return 0;
	}


    // here
	int nBufSize = 0, nLen = sizeof(nBufSize);
	if (::getsockopt(hSocket, SOL_SOCKET,
		SO_SNDBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
		printf("Send buffer size: %d\n", nBufSize);

	nBufSize = 0;
	nLen = sizeof(nBufSize);
	if (::getsockopt(hSocket, SOL_SOCKET,
		SO_RCVBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
		printf("Receive buffer size: %d\n", nBufSize);



	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}

```

---

## 그런데 Nagle알고리즘을 쓰지 말아야하는 경우는?

* 실시간 MMORPG게임의 경우 Nagle을 쓰면 안된다.

```cpp
// TCP_NODELAY옵션을 넣어준다
int nOpt = 1;
::setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY,
    (char*)&nOpt, sizeof(nOpt));
```
