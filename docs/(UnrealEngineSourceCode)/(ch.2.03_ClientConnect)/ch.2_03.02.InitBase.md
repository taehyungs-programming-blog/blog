---
layout: default
title: "03-02. InitBase"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
class UIpNetDriver : public UNetDriver
{
    // ...
    // 사실여기는 ServerListen에서도 다뤘다.

virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error) override
{
    if (!UNetDriver::InitBase(bInitAsClient, InNotify, URL, bReuseAddressAndPort, Error))
    {
        return false;
    }

    ISocketSubsystem* SocketSubsystem = GetSocketSubsystem();
    if (SocketSubsystem == nullptr)
    {
        return false;
    }

    // BindPort is assigned URL.Port (ServerPort) for server, GetClientPort() is called for client side
    // BindPort는 서버의 경우 URL.Port(ServerPort)가 할당되고, 클라이언트 측에서는 GetClientPort()가 호출됩니다
    const int32 BindPort = bInitAsClient ? GetClientPort() : URL.Port;

    // increase socket queue size, because we are polling rather than threading and thus we rely on the OS socket to buffer a lot of data
    // by default, Desired[Recv|Send]Size are determined like this:
    // - ClientDesiredSocketReceiveBufferBytes(ClientDesiredSocketSendBufferBytes): 32768(32KB)
    // - ServerDesiredSocketReceiveBufferBytes(ServerDesiredSocketSendBufferBytes): 131072(128KB)
    // - Send/Recv is same!

    // 소켓 큐 크기를 증가시킵니다. 스레딩 대신 폴링을 사용하므로 OS 소켓이 많은 데이터를 버퍼링하는 데 의존합니다
    // 기본적으로 Desired[Recv|Send]Size는 다음과 같이 결정됩니다:
    // - ClientDesiredSocketReceiveBufferBytes(ClientDesiredSocketSendBufferBytes): 32768(32KB)
    // - ServerDesiredSocketReceiveBufferBytes(ServerDesiredSocketSendBufferBytes): 131072(128KB)
    // - 송신/수신 크기는 동일합니다!

    const int32 DesiredRecvSize = bInitAsClient ? ClientDesiredSocketReceiveBufferBytes : ServerDesiredSocketReceiveBufferBytes;
    const int32 DesiredSendSize = bInitAsClient ? ClientDesiredSocketSendBufferBytes : ServerDesiredSocketSendBufferBytes;

    const EInitBindSocketsFlags InitBindFlags = bInitAsClient ? EInitBindSocketsFlags::Client : EInitBindSocketsFlags::Server;
    FCreateAndBindSocketFunc CreateAndBindSocketsFunc = [this, BindPort, bReuseAddressAndPort, DesiredRecvSize, DesiredSendSize]
        (TSharedRef<FInternetAddr> BindAddr, FString& Error) -> FUniqueSocket
        {
            return this->CreateAndBindSocket(BindAddr, BindPort, bReuseAddressAndPort, DesiredRecvSize, DesiredSendSize, Error);
        };
    
    bool bInitBindSocketsSuccess = Resolver->InitBindSockets(MoveTemp(CreateAndBindSocketsFunc), InitBindFlags, SocketSubsystem, Error);
    if (!bInitBindSocketsSuccess)
    {
        return false;
    }

    SetSocketAndLocalAddress(Resolver->GetFirstSocket());

    return true;
}
```

```cpp
class UIpConnection : public UNetConnection
{
    virtual void InitBase(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override
    {
        // see UNetConnection::InitBase
        // UNetConnection::InitBase 참조
        UNetConnection::InitBase(InDriver, InDocket, InURL, InState,
            (InMaxPacket == 0 || InMaxPacket > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : InMaxPacket,
            InPacketOverhead == 0 ? UDP_HEADER_SIZE : InPacketOverhead);
    }

    /** initialize this connection instance ***to*** a remote source */
    /** 이 연결 인스턴스를 원격 소스로 초기화 */
    virtual void InitLocalConnection(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override
    {
        InitBase(InDriver, InSocket, InURL, InState,
            // use the default packet size/overhead unless overriden by a child class
            // 자식 클래스에 의해 재정의되지 않는 한 기본 패킷 크기/오버헤드 사용

            // *** note that even though we put InMaxPacket larger than MAX_PACKET_SIZE, it just clamp it!
            // *** InMaxPacket을 MAX_PACKET_SIZE보다 크게 설정해도 그냥 제한됩니다!

            // - do NOT modify MAX_PACKET_SIZE arbitrarily!
            // - MAX_PACKET_SIZE를 임의로 수정하지 마세요!
            (InMaxPacket == 0 || InMaxPacket > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : InMaxPacket,
            InPacketOverhead == 0 ? UDP_HEADER_SIZE : InPacketOverhead);
        
    }
};
```

---

# Tips) UE에서 ID관리하기

```cpp
class FNetConnectionIdHandler
{
    FNetConnectionIdHandler()
        : IdHint(0)
    {
    }

    ~FNetConnectionIdHandler() {}

    // FNetConnectionIdHandler::Init() is called UNetDriver::PostInitProperties()
    void Init(uint32 IdCount)
    {
        check(IdCount > 0);
        UsedIds.Init(false, IdCount + 1);
        // treat 0 as an invalid connection ID; it's used by NetConnection CDOs
        UsedIds[0] = true;
    }

    uint32 Allocate()
    {
        uint32 Id = UsedIds.FindAndSetFirstZeroBit(IdHint);
        IdHint = Id + 1;
        return Id != INDEX_NONE ? Id : 0u;
    }

    void Free(uint32 Id)
    {
        UsedIds[Id] = (Id == 0u);
        IdHint = FMath::Min(IdHint, Id);
    }

    TBitArray<> UsedIds;
    uint32 IdHint;
};
```

`FNetConnectionIdHandler` 클래스는 네트워크 연결 ID를 관리하는 데 사용되는 클래스입니다. 주요 기능과 동작 방식은 다음과 같습니다:

1. 초기화:
```cpp
void Init(uint32 IdCount)
{
    check(IdCount > 0);
    UsedIds.Init(false, IdCount + 1);
    UsedIds[0] = true;
}
```
- `IdCount + 1` 크기의 비트 배열을 생성합니다.
- 0번 ID는 무효한 연결 ID로 취급하여 사용 중으로 표시합니다.

2. ID 할당:
```cpp
uint32 Allocate()
{
    uint32 Id = UsedIds.FindAndSetFirstZeroBit(IdHint);
    IdHint = Id + 1;
    return Id != INDEX_NONE ? Id : 0u;
}
```
- `IdHint`부터 시작하여 첫 번째 사용 가능한 ID를 찾아 할당합니다.
- 할당된 ID의 다음 값을 `IdHint`로 설정하여 다음 검색 시 효율성을 높입니다.
- 사용 가능한 ID가 없으면 0을 반환합니다.

3. ID 해제:
```cpp
void Free(uint32 Id)
{
    UsedIds[Id] = (Id == 0u);
    IdHint = FMath::Min(IdHint, Id);
}
```
- 지정된 ID를 사용 가능한 상태로 변경합니다 (0번 ID 제외).
- `IdHint`를 해제된 ID와 현재 `IdHint` 중 작은 값으로 업데이트하여 효율적인 재사용을 보장합니다.

이 클래스는 `TBitArray<>`를 사용하여 메모리를 효율적으로 사용합니다. 각 ID는 단일 비트로 표현되어, `TArray<int32>`를 사용하는 것보다 32배 더 적은 메모리를 사용합니다. 이는 많은 수의 연결을 관리해야 하는 네트워크 시스템에서 중요한 최적화입니다.