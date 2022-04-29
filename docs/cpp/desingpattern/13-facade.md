---
layout: default
title: "13. Facade Pattern"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Facade Pattern

🥑 서브 시스템을 합성하는 다수의 객체들의 인터페이스 집합에 대해 일관된 하나의 인터페이스를 제공<br>
🥑 Facade는 서브 시스템을 사용하기 쉽게 하기 위한 포괄적 개념의 인터페이스를 제공<br>

---

## Example

```cpp
int main() {
    // 1. 네트워크 라이브러리 초기화
    WSADATA w;
    WSAStartup(0x202, &w);

    // 2. 소켓 생성
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    // 3. 소켓에 주소 지정
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sock, (SOCKETADDR*)&addr, sizeof(addr));

    // 4. 소켓을 대기 상태로 변경
    listen(sock, 5);

    // 5. 클라이언트가 접속할때 까지 대기
    struct sockaddr_in addr2 = {0};
    int sz = sizeof(addr2);

    accept(sock, (SOCKADDR*)&addr2, &sz);

    // 6. socket 라이브러리 cleanup
    WSACleanup()
}
```

🥑 멋지 C코드긴 한데 … C++ 스럽게 바꿔보자.

```cpp
// network 라이브러리의 초기화 cleanup 담당.
class NetworkInit
{
public:
    NetworkInit()
    {
        WSADATA w;
        WSAStartup(0x202, &w);
    }
    ~NetworkInit()
    {
        WSACleanup();
    }
};
```

```cpp
// UP 주소 관리
class IPAddress
{
    struct sockaddr_in addr;
public:
    IPAddress(const char* ip, short port)
    {
        addr.sin_family = AF_INET;
        addr.sin_port = htos(port);
        addr.sin_addr.s_addr = inet_addr(ip);
    }
    struct sockaddr* getRawAddress() { return (struct sockaddr*)&addr; }
};
```

```cpp
// socket 프로그래밍의 일반적 절차
class Socket
{
    int sock;
public:
    Socket(int type) { sock = socket(PF_INET, type, 0); }
    void Bind(IPAddress* ip)
    {
        ::bind(sock, )      // ...
    }
};
```

```cpp
int main()
{
    NetworkInit init;
    IPAddress ip("127.0.0.1", 4000);
    Socket sock(SOCK_STREAM);
    sock.Bind(&ip);
    sock.Listen();
    sock.Accept();
}
```

🥑 흐음 … 그래도 위 절차를 다 알아야 사용이 가능하겠네? 좀 더 간단히 못하나??

```cpp
class TCPServer
{
    NetworkInit init;
    Socket sock;
public:
    TCPServer() : sock(SOCK_STREAM) {}
    void Start(const char* sip, short port)
    {
        IPAddress ip(sip, port);
        sock.Bind(&sip);
        sock.Listen();
        sock.Accept();
    }
};

int main()
{
    TCPServer server;
    server.Start("127.0.0.1", 4000);
}
```