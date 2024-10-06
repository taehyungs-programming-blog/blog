---
layout: default
title: "01-03. FSocketWindows"
parent: "([Network] 01.SocketSubsystemModule)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
/** implements a Windows/BSD network socket */
//  FSocketWindows is just simple wrapper of SocketBSD, see FSocketBSD 
class FSocketWindows : public FSocketBSD
{
    FSocketWindows(SOCKET InSocket, ESocketType InSocketType, const FString& InSocketDescription, const FName& InSocketProtocol, ISocketSubsystem* InSubsystem)
	    : FSocketBSD(InSocket, InSocketType, InSocketDescription, InSocketProtocol, InSubsystem)
    {}

    // ...

    /** holds the BSD socket object */
    // platform-dependent socket handle (BSD)
    SOCKET Socket;

    /** flags which will be passed to send and sendto */
    int32 SendFlags = 0;

    /** pointer to the subsystem that created it */
    ISocketSubsystem* SocketSubsystem;
};
```

```cpp
class FSocketBSD : public FSocket
```

```cpp
/** this is our abstract base class that hides the platform specific socket implementation */
/** 이것은 플랫폼별 소켓 구현을 숨기는 추상 기본 클래스입니다 *
class FSocket
{
    /** indices the type of socket this is */
    /** 이 소켓의 유형을 나타냅니다 */
    // it has type identifier (e.g. TCP(SOCK_TYPE_Datagram) or UDP(SOCK_TYPE_Streaming))
    // 소켓 유형 식별자를 가집니다 (예: TCP(SOCK_TYPE_Datagram) 또는 UDP(SOCK_TYPE_Streaming))
    const ESocketType SocketType;

    /** debug description of socket usage */
    /** 소켓 사용에 대한 디버그 설명 */
    FString SocketDescription;

    /** protocol used in creation of a socket */
    /** 소켓 생성에 사용된 프로토콜 */
    // IPv4 or IPv6
    // IPv4 또는 IPv6
    FName SocketProtocol;
};
```

* 함수를 좀 살펴볼까

```cpp
virtual void GetAddress(FInternetAddr& OutAddr) override
{
    // FInternetAddrBSD 관련해 다음장에서 설명.
    FInternetAddrBSD& BSDAddr = static_cast<FInternetAddrBSD&>(OutAddr);
    SOCKLEN Size = sizeof(sockaddr_storage);

    // figure out what ip/port we are bound to
    // using getsockname(), we can retrieve address binded in Socket(socket handle)
    bool bOk = getsockname(Socket, (sockaddr*)&BSDAddr.Addr, &Size) == 0;
    if (bOk == false)
    {
        check(SocketSubsystem);
    }
}
```

```cpp
// the wrapper of UDP's sendto 
// UDP의 sendto 함수를 감싸는 래퍼
virtual bool SendTo(const uint8* Data, int32 Count, int32& BytesSent, const FInternetAddr& Destination) override
{
    // as we covered, the UDP send protocol is very simple just set address and send it and DONE!
    // 우리가 다뤘듯이, UDP 전송 프로토콜은 매우 간단합니다. 주소를 설정하고 전송하면 끝입니다!
    const FInternetAddrBSD& BSDAddr = static_cast<const FInternetAddrBSD&>(Destination);

    // write the data and see how much was written
    // 데이터를 쓰고 얼마나 쓰여졌는지 확인합니다
    // call sendto()
    // sendto() 호출
    // - note that Count's unit is 'BYTE'
    // - Count의 단위가 '바이트'임에 주의하세요
    // - SendFlags is usually 'None(==0)'
    // - SendFlags는 보통 'None(==0)'입니다
    BytesSent = sendto(Socket, (const char*)Data, Count, SendFlags, (const sockaddr*)&(BSDAddr.Addr), BSDAddr.GetStorageSize());

    return BytesSent >= 0;
}
```

```cpp
virtual bool RecvFrom(uint8* Data, int32 BufferSize, int32& BytesRead, FInternetAddr& Source, ESocketReceiveFlags::Type Flags = ESocketReceiveFlags::None) override
{
    bool bSuccess = false;
    const bool bStreamSocket = (SocketType == SOCKTYPE_Streaming);

    // usually Flags is '0: None', it is useful to know some flags used in recvfrom()
    // 보통 Flags는 '0: None'입니다. recvfrom()에서 사용되는 일부 플래그를 아는 것이 유용합니다.
    const int32 TranslatedFlags = TranslateFlags(Flags);

    FInternetAddrBSD& BSDAddr = static_cast<FInternetAddrBSD&>(Source);
    SOCKLEN Size = sizeof(sockaddr_storage);
    sockaddr* Addr = (sockaddr*)&(BSDAddr.Addr);

    // read into the buffer and set the source address
    // 버퍼로 읽고 소스 주소를 설정합니다
    // recvfrom call: UDP recv
    // recvfrom 호출: UDP 수신
    BytesRead = recvfrom(Socket, (char*)Data, BufferSize, TranslatedFlags, Addr, &Size);
    if (BytesRead >= 0)
    {
        bSuccess = !bStreamSocket || (BytesRead > 0);
    }

    return bSuccess;
}
```

```cpp
// since the flag constants may have different values per-platform, translate into corresponding system constants
// for example, MSG_WAITALL is 0x8 on windows, but 0x100 on other platform
// 플랫폼마다 플래그 상수 값이 다를 수 있으므로, 해당 시스템의 상수로 변환합니다.
// 예를 들어, MSG_WAITALL은 윈도우에서는 0x8이지만 다른 플랫폼에서는 0x100입니다.
inline int TranslateFlags(ESocketReceiveFlags::Type Flags)
{
    int TranslatedFlags = 0;

    // like typical messaging system (e.g. windows messages), it has peek/wait-all method
    // 일반적인 메시징 시스템(예: 윈도우 메시지)처럼 peek/wait-all 메서드가 있습니다.
    if (Flags & ESocketReceiveFlag::Peek)
    {
        TranslatedFlags |= MSG_PEEK;
    }

    // MSG_WAITALL is like blocking receving (until read all requested bytes, just wait)
    // MSG_WAITALL은 블로킹 수신과 같습니다 (요청된 모든 바이트를 읽을 때까지 대기)
    if (Flags & ESocketReceiveFlags::WaitAll)
    {
        TranslatedFlags |= MSG_WAITALL;
    }

    return TranslatedFlags;
}
```

* 여기까지 하면 SocketSubsystemModule의 생성은 완료가 된다!