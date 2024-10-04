---
layout: default
title: "03-01. InitBase"
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
virtual void InitBase(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override
{
    UNetConnection::InitBase(InDriver, InDocket, InURL, InState,
        (InMaxPacket == 0 || InMaxPacket > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : InMaxPacket,
        InPacketOverhead == 0 ? UDP_HEADER_SIZE : InPacketOverhead);
}
```

```cpp
/** initialize common settings for this connection instance */
virtual void InitBase(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0)
{
    // owning NetDriver
    // see member variable Driver breifly
    Driver = InDriver;

    // it looks like UNetConnection holds UChannel related data:
    // - see InitChannelData (***)
    InitChannelData();

    // NetDriver allocate connection id:
    SetConnectionId(InDriver->AllocateConnectionId());

    // our state is SOCK_PENDING, when we successfully handshake process, it will be SOCK_OPEN (connection is open)
    State = InState;
    MaxPacket = InMaxPacket;

    // create package map
    // we are not going to see the detail of UPackageMapClient for now
    // - but, in terms of replication, PackageMapClient is very important object
    // *** just remember each NetConnection has its own PacketMapClient
    UPackageMapClient* PackageMapClient = NewObject<UPackageMapClient>(this, PackageMapClass);
    {
        PackageMapClient->Initialize(this, Driver->GuidCache);
        PackageMap = PackageMapClient;
    }

    // initialize handler
    // now we finally reach to Handler!
    // - see member variable Handler (PacketHandler)
    // - in server-side, we make PacketHandler and HandlerComponent(StatelessConnectHandlerComponent)
    //   - in cliend-side, NetConnection has PacketHandler matching HandlerComponent(StatelessConnectHandlerComponent)
    {
        Handler.Reset();

        InitHandler();
    }

    // goto UIpConnection::InitBase()
}
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