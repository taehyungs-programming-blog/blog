---
layout: default
title: "02-01. UML을 통해 메시지 수신 프로세스 이해하기"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 대략 코드는 아래와 같고 상세한건 이후에 설명 예정입니다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_1_1.png"/>
</p>

```cpp
void UWorld::Tick()
{
    // ... 생략 ...
    BroadcastTickDispatch();
    // ... 생략 ...
}

void UWorld::BroadcastTickDispatch()
{
    for (FNamedNetDriver& NamedNetDriver : NetDrivers)
    {
        if (NamedNetDriver.NetDriver)
        {
            NamedNetDriver.NetDriver->InternalTickDispatch();
        }
    }
}
```

```cpp
void UIpNetDriver::InternalTickDispatch()
{
    TickDispatch();
}

void UIpNetDriver::TickDispatch()
{
    uint8 Data[MAX_PACKET_SIZE];
    int32 BytesRead = 0;
    
    // 소켓으로부터 데이터 수신
    while (Socket->RecvFrom(Data, sizeof(Data), BytesRead, ...))
    {
        ProcessReceivedPacket(Data, BytesRead, ...);
    }
}

void UIpNetDriver::ProcessReceivedPacket(uint8* Data, int32 DataSize, ...)
{
    // Connection 찾기
    UNetConnection* Connection = FindConnectionForPacket(...);

    if (!Connection)
    {
        // Connectionless 패킷 처리
        ProcessConnectionlessPacket(Data, DataSize, ...);
    }
    else
    {
        // 연결된 Connection의 패킷 처리
        Connection->ReceivedPacket(Data, DataSize);
    }
}

void UIpNetDriver::ProcessConnectionlessPacket(uint8* Data, int32 DataSize, ...)
{
    if (PacketHandler.IsValid())
    {
        PacketHandler->Incoming(Data, DataSize, ...);
    }
}
```

```cpp
void UNetConnection::ReceivedPacket(uint8* Data, int32 DataSize)
{
    // PacketHandler 처리
    if (Driver->PacketHandler.IsValid())
    {
        bool bProcessed = Driver->PacketHandler->Incoming(Data, DataSize, ...);
        if (bProcessed)
        {
            return;
        }
    }

    ProcessReceivedPacket(Data, DataSize);
}

void UNetConnection::ProcessReceivedPacket(uint8* Data, int32 DataSize)
{
    FBitReader Reader(Data, DataSize * 8);
    
    if (IsControlMessage(Reader))
    {
        uint8 MessageType = Reader.ReadBit() ? 1 : 0;
        FInBunch Bunch(this, Reader);
        Driver->World->NotifyControlMessage(this, MessageType, Bunch);
    }
}
```

```cpp
void PacketHandler::Incoming(uint8* Data, int32 CountBytes, ...)
{
    return Incoming_Internal(Data, CountBytes, ...);
}

void PacketHandler::Incoming_Internal(uint8* Data, int32 CountBytes, ...)
{
    FBitReader Reader(Data, CountBytes * 8);

    // 각 HandlerComponent에서 패킷 처리 시도
    for (int32 i = HandlerComponents.Num() - 1; i >= 0; i--)
    {
        UHandlerComponent* Component = HandlerComponents[i];
        if (Component->IsValid())
        {
            if (Component->IncomingConnectionless(Reader, ...))
            {
                return true;
            }
        }
    }
}
```

```cpp
bool StatelessConnectHandlerComponent::IncomingConnectionless(FBitReader& Packet, ...)
{
    uint8 MessageType = Packet.ReadBit() ? 1 : 0;
    
    switch (MessageType)
    {
        case NMT_Hello:
            return ProcessConnectionlessPacket(Packet, ...);
        case NMT_Challenge:
            return ProcessChallengeResponse(Packet, ...);
        // ... 기타 메시지 타입 처리 ...
    }
}
```

```cpp
void UWorld::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch)
{
    switch (MessageType)
    {
        case NMT_Hello:
            // 초기 연결 요청 처리
            break;
        case NMT_Challenge:
            // 챌린지 응답 처리
            break;
        case NMT_BeaconWelcome:
            // 비콘 웰컴 메시지 처리
            break;
    }
}
```

---

## Tip) PlantUML 코드

```
@startuml
skinparam responseMessageBelowArrow true

participant UWorld
participant UIpNetDriver
participant UNetConnection
participant PacketHandler
participant StatelessConnectHandlerComponent

UWorld -> UWorld: Tick()
activate UWorld

UWorld -> UWorld: BroadcastTickDispatch()
activate UWorld

UWorld -> UIpNetDriver: InternalTickDispatch()
activate UIpNetDriver

UIpNetDriver -> UIpNetDriver: TickDispatch()
activate UIpNetDriver
note right: Socket에서 패킷 수신

UIpNetDriver -> UIpNetDriver: ProcessReceivedPacket()
activate UIpNetDriver

alt Connection 없음 (Connectionless 패킷)
    UIpNetDriver -> UIpNetDriver: ProcessConnectionlessPacket()
    activate UIpNetDriver
    
    UIpNetDriver -> PacketHandler: Incoming()
    activate PacketHandler
    
    PacketHandler -> PacketHandler: Incoming_Internal()
    activate PacketHandler
    
    PacketHandler -> StatelessConnectHandlerComponent: IncomingConnectionless()
    activate StatelessConnectHandlerComponent
    note right: NMT_Hello, NMT_Challenge 등 처리

else Connection 있음 (일반 패킷)
    UIpNetDriver -> UNetConnection: ReceivedPacket()
    activate UNetConnection
    
    UNetConnection -> PacketHandler: Incoming()
    activate PacketHandler
    
    alt 패킷 핸들러가 처리하지 않은 경우
        UNetConnection -> UNetConnection: ProcessReceivedPacket()
        activate UNetConnection
        
        UNetConnection -> UWorld: NotifyControlMessage()
        activate UWorld
        note right: 컨트롤 메시지 처리
    end
end

@enduml
```