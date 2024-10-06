---
layout: default
title: "01-02. FSocketSubsystemWindows"
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
// FSocketSubsystemWindows에 대해 좀 살펴보자
class FSocketSubsystemWindows : public FSocketSubsystemBSD
{
    // ...


    /** holds a flag indicating whether Init() has been called before or not */
        /** Init()가 이전에 호출되었는지 여부를 나타내는 플래그를 보유 */
    // it is one of the usual pattern in UE:
    // - if you want to call only once for specific function, add the calling state and prevent it from calling multiple times

    // 이것은 UE에서 흔히 볼 수 있는 패턴 중 하나입니다:
    // - 특정 함수를 한 번만 호출하고 싶다면, 호출 상태를 추가하고 여러 번 호출되는 것을 방지합니다
    bool bTriedToInit;

    /** holds the single instantiation of this subsystem */
    /** 이 서브시스템의 단일 인스턴스를 보유 */
    // SocketSubsystem maintain its singularity by singleton pattern
    // SocketSubsystem은 싱글톤 패턴을 통해 유일성을 유지합니다
    static FSocketSubsystemWindows* SocketSingleton;
```

```cpp
// 003 - Networking - SocketSubsystemModule * - FSocketSubsystemBSD
//  BSD means unix-socket
// - you'll notice that Unreal uses bsd-socket(unix) features in windows socket
// - in other words, the Unreal doesn't use unique feature in windows socket
// - so, the SocketSubsystemWindows is based on SocketSubsystemBSD

// 003 - 네트워킹 - SocketSubsystemModule * - FSocketSubsystemBSD
//  BSD는 유닉스 소켓을 의미합니다
// - 언리얼이 윈도우 소켓에서 bsd-소켓(유닉스) 기능을 사용하는 것을 알 수 있습니다
// - 다시 말해, 언리얼은 윈도우 소켓의 고유한 기능을 사용하지 않습니다
// - 따라서 SocketSubsystemWindows는 SocketSubsystemBSD를 기반으로 합니다
class FSocketSubsystemBSD : public ISocketSubsystem
```

* 함수를 좀 살벼볼까?

```cpp
static FSocketSubsystemWindows* Create()
{
    if (SocketSingleton == nullptr)
    {
        // see briefly FSocketSubsystemWindows's constructor
        SocketSingleton = new FSocketSubsystemWindows();
    }
    return SocketSingleton;
}
```

```cpp
static void Destory()
{
    if (SocketSingleton != nullptr)
    {
        SocketSingleton->Shutdown();
        delete SocketSingleton;
        SocketSingleton = nullptr;
    }
}
```

```cpp
virtual bool Init(FString& Error) override
{
    bool bSuccess = false;
    if (bTriedToInit == false)
    {
        bTriedToInit = true;

        // initialize WSA (Windows Socket API)
        // 윈도우 소켓 API 초기화
        // in windows platform, we call WSAStartup to indicate whether this app use socket or not
        // 윈도우 플랫폼에서는 WSAStartup을 호출하여 이 앱이 소켓을 사용하는지 여부를 나타냅니다
        // - you should match WSACleanup (see below Shutdown() method)
        // - Shutdown() 메서드에서 WSACleanup과 짝을 이뤄야 합니다 (아래 참조)
        // - as you can see first parameter, we request 1.1 version (not 2.2)
        // - 첫 번째 매개변수에서 볼 수 있듯이, 우리는 1.1 버전을 요청합니다 (2.2가 아님)
        //   - 0x0101 is same as MAKEWORD(2,2) which is typically used in socket programming in windows
        //   - 0x0101은 MAKEWORD(2,2)와 같으며, 이는 일반적으로 윈도우 소켓 프로그래밍에서 사용됩니다
        //     - 0x0202 is MAKEWORD(2,2)
        //     - 0x0202는 MAKEWORD(2,2)입니다
        // from here, we know that UE uses low-version of socket library 1.1
        // 여기서 우리는 UE가 낮은 버전의 소켓 라이브러리 1.1을 사용한다는 것을 알 수 있습니다
        // 왜 그런지는 이후에 설명.
        WSADATA WSAData;
        int32 Code = WSAStartup(0x0101, &WSAData);
        if (Code == 0)
        {
            bSuccess = true;
        }
    }

    return bSuccess;
}
```

```cpp
virtual class FSocket* CreateSocket(const FName& SocketType, const FString& SocketDescription, const FName& ProtocolType) override
{
    // we try to create socket by SocketSubsystemBSD
        // SocketSubsystemBSD를 사용하여 소켓을 생성하려고 합니다
    FSocketWindows* NewSocket = (FSocketWindows*)FSocketSubsystemBSD::CreateSocket(SocketType, SocketDescription, ProtocolType);
    return NewSocket;


    // let see basic functionalities(GetAddress, SendTo, RecvFrom) for FSocketBSD
    // - note that SendTo/RecvFrom is UDP dedicated methods (recall sendto/recvfrom is UDP functions)

    // FSocketBSD의 기본 기능(GetAddress, SendTo, RecvFrom)을 살펴봅시다
    // - SendTo/RecvFrom은 UDP 전용 메서드임을 주의하세요 (sendto/recvfrom이 UDP 함수임을 상기하세요)
}
```

```cpp
virtual FSocket* CreateSocket(const FName& SocketType, const FString& SocketDescription, const FName& ProtocolType) override
{
    // SOCKET is the handle of socket which is provided by the OS(e.g. linux, windows, mac, ...)
        // SOCKET은 OS(예: 리눅스, 윈도우, 맥 등)에서 제공하는 소켓의 핸들입니다.
    SOCKET Socket = INVALID_SOCKET;

    FSocket* NewSocket = nullptr;

    FName SocketProtocolType = ProtocolType;

    // in our networking course, the only thing we are interested in is NAME_DGram(UDP)
    // - the only functionalities for socket is 'UDP' socket
        // 우리의 네트워킹 과정에서 우리가 관심 있는 유일한 것은 NAME_DGram(UDP)입니다.
        // - 소켓의 유일한 기능은 'UDP' 소켓입니다.
    bool bIsUDP = SocketType.GetComparisonIndex() == NAME_DGram;

    // naturally, we expect SocketTypeFlag is SOCK_DGRAM:
    // - TCP is SOCK_STREAM, and UDP is SOCK_DGRAM
        // 당연히 우리는 SocketTypeFlag가 SOCK_DGRAM일 것으로 예상합니다:
        // - TCP는 SOCK_STREAM이고, UDP는 SOCK_DGRAM입니다.
    int32 SocketTypeFlag = (bIsUDP) ? SOCK_DGRAM : SOCK_STREAM;

    // we create Socket:
    // - see GetProtocolFamilyValue 
        // 우리는 Socket을 생성합니다:
        // - GetProtocolFamilyValue를 참조하세요 
    Socket = socket(GetProtocolFamilyValue(SocketProtocolType), SocketTypeFlag, ((bIsUDP) ? IPPROTO_UDP : IPPROTO_TCP));
    {
        // we create UE Socket object by InternalBSDSocketFactory
        // - note that InternalBSDSocketFactory is called by FSocketSubsystemWindows::InternalBSDSocketFactory not FSocketSubsystemBSD::InternalBSDSocketFactory

        // InternalBSDSocketFactory를 통해 UE Socket 객체를 생성합니다
        // - InternalBSDSocketFactory는 FSocketSubsystemBSD::InternalBSDSocketFactory가 아닌 FSocketSubsystemWindows::InternalBSDSocketFactory에 의해 호출된다는 점에 주목하세요
        NewSocket = (Socket != INVALID_SOCKET) ? InternalBSDSocketFactory(Socket, ((bIsUDP) ? SOCKTYPE_Datagram : SOCKTYPE_Streaming), SocketDescription, SocketProtocolType) : nullptr;
    }

    return NewSocket;
}
```

```cpp
virtual FSocketBSD* InternalBSDSocketFactory(SOCKET Socket, ESocketType SocketType, const FString& SocketDescription, const FName& SocketProtocol) override
{
    // now it is time to see unreal socket object!
    // - see FSocketWindows
    return new FSocketWindows(Socket, SocketType, SocketDescription, SocketProtocol, this);
}
```