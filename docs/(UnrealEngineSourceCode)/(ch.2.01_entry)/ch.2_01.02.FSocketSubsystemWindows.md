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
/** Windows specific socket subsystem implementation */
// see FSocketSubsystemBSD first
class FSocketSubsystemWindows : public FSocketSubsystemBSD
{
    // ...

    /** singleton interface for this subsystem */
    // note that it is 'singleton pattern'
    static FSocketSubsystemWindows* Create()
    {
        if (SocketSingleton == nullptr)
        {
            SocketSingleton = new FSocketSubsystemWindows();
        }
        return SocketSingleton;
    }

    virtual bool Init(FString& Error) override
    {
        bool bSuccess = false;
        if (bTriedToInit == false)
        {
            bTriedToInit = true;

            // initialize WSA (Windows Socket API)
            // in windows platform, we call WSAStartup to indicate whether this app use socket or not
            // - you should match WSACleanup (see below Shutdown() method)
            // - as you can see first parameter, we request 1.1 version (not 2.2)
            //   - 0x0101 is same as MAKEWORD(2,2) which is typically used in socket programming in windows
            //     - 0x0202 is MAKEWORD(2,2)
            // from here, we know that UE uses low-version of socket library 1.1
            WSADATA WSAData;
            int32 Code = WSAStartup(0x0101, &WSAData);
            if (Code == 0)
            {
                bSuccess = true;
            }
        }

        return bSuccess;
    }

    virtual class FSocket* CreateSocket(const FName& SocketType, const FString& SocketDescription, const FName& ProtocolType) override
    {
        // we try to create socket by SocketSubsystemBSD
        FSocketWindows* NewSocket = (FSocketWindows*)FSocketSubsystemBSD::CreateSocket(SocketType, SocketDescription, ProtocolType);
        return NewSocket;

        // let see basic functionalities(GetAddress, SendTo, RecvFrom) for FSocketBSD
        // - note that SendTo/RecvFrom is UDP dedicated methods (recall sendto/recvfrom is UDP functions)
    }

    /** holds a flag indicating whether Init() has been called before or not */
    // it is one of the usual pattern in UE:
    // - if you want to call only once for specific function, add the calling state and prevent it from calling multiple times
    bool bTriedToInit;

    /** holds the single instantiation of this subsystem */
    // SocketSubsystem maintain its singularity by singleton pattern
    static FSocketSubsystemWindows* SocketSingleton;
```

```cpp
virtual FSocket* CreateSocket(const FName& SocketType, const FString& SocketDescription, const FName& ProtocolType) override
{
    // SOCKET is the handle of socket which is provided by the OS(e.g. linux, windows, mac, ...)
    SOCKET Socket = INVALID_SOCKET;

    FSocket* NewSocket = nullptr;

    FName SocketProtocolType = ProtocolType;

    // in our networking course, the only thing we are interested in is NAME_DGram(UDP)
    // - the only functionalities for socket is 'UDP' socket
    bool bIsUDP = SocketType.GetComparisonIndex() == NAME_DGram;

    // naturally, we expect SocketTypeFlag is SOCK_DGRAM:
    // - TCP is SOCK_STREAM, and UDP is SOCK_DGRAM
    int32 SocketTypeFlag = (bIsUDP) ? SOCK_DGRAM : SOCK_STREAM;

    // we create Socket:
    Socket = socket(GetProtocolFamilyValue(SocketProtocolType), SocketTypeFlag, ((bIsUDP) ? IPPROTO_UDP : IPPROTO_TCP));
    {
        // we create UE Socket object by InternalBSDSocketFactory
        // - note that InternalBSDSocketFactory is called by FSocketSubsystemWindows::InternalBSDSocketFactory not FSocketSubsystemBSD::InternalBSDSocketFactory
        NewSocket = (Socket != INVALID_SOCKET) ? InternalBSDSocketFactory(Socket, ((bIsUDP) ? SOCKTYPE_Datagram : SOCKTYPE_Streaming), SocketDescription, SocketProtocolType) : nullptr;
    }

    return NewSocket;
}
```

```cpp
// BSD means unix-socket
// - you'll notice that Unreal uses bsd-socket(unix) features in windows socket
// - in other words, the Unreal doesn't use unique feature in windows socket
// - so, the SocketSubsystemWindows is based on SocketSubsystemBSD
class FSocketSubsystemBSD : public ISocketSubsystem
{
```