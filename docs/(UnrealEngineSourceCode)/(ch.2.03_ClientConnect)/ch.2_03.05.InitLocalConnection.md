---
layout: default
title: "03-05. InitLocalConnection"
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
/** initialize this connection instance ***to*** a remote source */
// 원격 소스에 대한 이 연결 인스턴스를 초기화합니다
virtual void InitLocalConnection(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override
{
    InitBase(InDriver, InSocket, InURL, InState,
        (InMaxPacket == 0 || InMaxPacket > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : InMaxPacket,
        InPacketOverhead == 0 ? UDP_HEADER_SIZE : InPacketOverhead);
    
    // see where Resolver(NetConnectionAddressResolution) is initialized (UIpConnection::UIpConnection)
    // - note that FNetConnectionAddressResolution is managed by FNetDriverAddressResolution:
    // - we can understand the relationship like this:

    // Resolver(NetConnectionAddressResolution)가 초기화되는 위치를 확인하세요 (UIpConnection::UIpConnection)
    // - FNetConnectionAddressResolution은 FNetDriverAddressResolution에 의해 관리된다는 점에 주목하세요:
    // - 우리는 이 관계를 다음과 같이 이해할 수 있습니다:
    //                                                             *** in general, NetDriverAddressResolution has socket to communicate                                                                      
    //                                                             *** 일반적으로 NetDriverAddressResolution은 통신을 위한 소켓을 가지고 있습니다
    //                                                ┌──────────────────────────────────────────────────────────┐                                                                                   
    //                                                │                                                          │                                                                                   
    //      ──────────────────────────────────────────┴───────────                                               │                                                                                   
    //   ┌──FNetDriverAddressResolution : UNetDriver(UIpNetDriver)─────────────────────┐                         │                                                                                   
    //   │                                                                             │                         │                                                                                   
    //   │    ┌──────────────────────────────────────────────────────────────────────┐ │                         │                                                                                   
    //   │    │ FNetConnectionAddressResolution : UNetConnection(UIpConnection) -- 0 ◄─┼─────────┐               │                                                                                   
    //   │    └──────────────────────────────────────────────────────────────────────┘ │         │               │                                                                                   
    //   │                                                                             │         ├───────────────┘                                                                                   
    //   │    ┌──────────────────────────────────────────────────────────────────────┐ │         │     NetDriverAddressResolution's BoundSockets are broadcasted to NetConnectionAddressResolutions  
    //   │    │ FNetConnectionAddressResolution : UNetConnection(UIpConnection) -- 1 ◄─┼─────────┘     NetDriverAddressResolution의 BoundSockets가 NetConnectionAddressResolutions로 브로드캐스트됩니다
    //   │    └──────────────────────────────────────────────────────────────────────┘ │                                                                                                             
    //   │                                                                             │                                                                                                             
    //   └─────────────────────────────────────────────────────────────────────────────┘                                                                                                             
    //  

    bool bResolverInit = Resolver->InitLocalConnection(InDriver->GetSocketSubsystem(), InSocket, InURL);
    if (!ResolverInit)
    {
        Close();
        return;
    }

    // UNetConnection::RemoteAddr cached TSharedPtr<FInternetAddr>
    // - RemoteAddr point to same memory address with FNetConnectionAddressResolution::RemoteAddr
    // - currently RemoteAddr is NOT resolved yet, but after resolution is finished, UNetConnection::RemoteAddr can access the resolved InternetAddr value
    
    // UNetConnection::RemoteAddr는 TSharedPtr<FInternetAddr>를 캐시합니다
    // - RemoteAddr는 FNetConnectionAddressResolution::RemoteAddr와 동일한 메모리 주소를 가리킵니다
    // - 현재 RemoteAddr는 아직 해결되지 않았지만, 해결이 완료되면 UNetConnection::RemoteAddr는 해결된 InternetAddr 값에 접근할 수 있습니다
    RemoteAddr = Resolver->GetRemoteAddr();

    // 아래 참조 InitSendBuffer (1)
    InitSendBuffer();
}
```

## (1) InitSendBuffer

```cpp
/** resets the FBitWriter to its default state */
/** FBitWriter를 기본 상태로 재설정합니다 */
virtual void InitSendBuffer()
{
    // FinalBufferSize is the data size which we can put in:
    // - MaxPacket is 1024
    // - MaxPacketHandlerBits is updated in InitHandler() by PacketHandler::GetTotalReservedPacketBits()
    // - FinalBufferSize is in unit 'Bits'

    // FinalBufferSize는 우리가 넣을 수 있는 데이터 크기입니다:
    // - MaxPacket은 1024입니다
    // - MaxPacketHandlerBits는 InitHandler()에서 PacketHandler::GetTotalReservedPacketBits()에 의해 업데이트됩니다
    // - FinalBufferSize의 단위는 '비트'입니다
    int32 FinalBufferSize = (MaxPacket * 8) - MaxPacketHandlerBits;

    // initialize the one outgoing buffer
    // 하나의 송신 버퍼를 초기화합니다
    // *** see UNetConnection::SendBuffer
    // *** UNetConnection::SendBuffer를 참조하세요
    if (FinalBufferSize == SendBuffer.GetMaxBits())
    {
        // reset all of our values to their initial state without a malloc/free
        // malloc/free 없이 모든 값을 초기 상태로 재설정합니다
        // see FBitWriter::Rest() briefly (similar to its constructor)
        // FBitWriter::Rest()를 간단히 살펴보세요 (생성자와 유사합니다)
        SendBuffer.Reset();
    }
    else
    {
        // first time initialization needs to allocate the buffer
        // 첫 번째 초기화 시 버퍼를 할당해야 합니다
        SendBuffer = FBitWriter(FinalBufferSize);
    }

    // see ResetPacketBitCounts briefly, we'll see these variables frequently
    // ResetPacketBitCounts를 간단히 살펴보세요, 이 변수들을 자주 보게 될 것입니다
    ResetPacketBitCounts();
}
```