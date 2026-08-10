---
layout: default
title: "04. socket option"
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

* 모두 다 설명은 불가능 하고 대략 이런것이 있다고 받아들이며 더 자세한 사항은 [MSDN](https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt)참고

```cpp
int setsockopt(
    SOCKET     s,         // 세팅할 소켓
    int        level,     // 1
    int        optname,   // 2
    const char *optval,   // 3
    int        optlen     // 4
);
```

### level

```cpp
// 1. level
// 옵션을 해석하고 처리할 주체
SOL_SOCKET      // 소켓 코드
IPPROTO_IP      // IPv4
IPPROTO_TCP     // TCP 프로토콜
```

### optname

```cpp
// 2. 어떤옵션을 선택할 것인가? (매우 많기에 많이 쓰는거만 정리)
SO_KEEPALIVE    //  주기적으로 연결상태 확인

// Example
bool enable = true;
::setsocketopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable sizeof(enable));

/* ************************ */

SO_LINGER       // send후 바로 closesocket을 한다면, send buf에 남아있는 데이터를 어떻게 처리할 것인가?
// Example
LINGER linger;
linger.l_onoff = 1;         // 바로 소켓을 닫지말고
linger.l_linger = 5;        // 5초대기
::setsocketopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger sizeof(linger));

/* ************************ */

// (추가)
// Half-Close(shutdown) - 그냥 closesocket을 하지말고 closesocket전 특별한 처리를 해둔다
// SD_SEND : send를 막음
// SD_RECEIVE : recv를 막음
// SD_BOTH : send, recv모두 막음
::shutdown(serverSocker, SD_SEND);

/* ************************ */

SO_SNDBUF       // 송신 버퍼 크기
SO_REVBUF       // 수신 버퍼 크기
int32 sendBufferSize;
int32 optionLen;
::getsockopt(serverSocket, SOL_SOCKET, SD_SNDBUF, (char*)&sendBufferSize, &optionLen);

/* ************************ */

SO_REUSEADDR         // IP 및 PORT 재활용
// 내가 bind하려던 주소가 이전에 사용, 비정상 상황으로 점유중일경우 바로사용못하는 경우가 있음
// 바로사용하게 해달라는 명령
bool enable = true;
::setsocketopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));

/* ************************ */

TCP_NODELAY     // Nagle 알고리즘 작동여부
// 데이터가 충분히 크면 보내고, 그렇지 않으면 데이터가 충분히 쌓일때까지 대기
// 장점 : 작은 패킷이 불필요하게 많이 생성되지 않음
// 단점 : 반응 시간 손해
// 게임은 보통 끈다
bool enable = true;
::setsocketopt(serverSocket, IPPROTO_TCP, TCP_NODELY, (char*)&enable, sizeof(enable));
// 보통 게임에선 send를 개발자가 원하는 만큼 쌓고 직접 send를보내지 Nagle알고리즘에 의존하지 않음.
```
