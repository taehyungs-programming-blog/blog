---
layout: default
title: "[MSG] NMT_Challenge"
parent: "([Network] 03. UE Net의 주요 메시지 정리)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_3_1.png"/>
</p>

```cpp
void UIpNetDriver::TickDispatch()
{
    bool bProcessedPacket = false;
    TSharedRef<FInternetAddr> FromAddr = GetSocketSubsystem()->CreateInternetAddr();

    // 소켓으로부터 모든 대기 중인 패킷을 처리
    while (bProcessedPacket)
    {
        uint8 Data[MAX_PACKET_SIZE];
        int32 BytesRead = 0;
        bProcessedPacket = false;

        // 소켓으로부터 데이터 읽기
        if (Socket->RecvFrom(Data, sizeof(Data), BytesRead, *FromAddr))
        {
            bProcessedPacket = true;
            
            // 수신된 패킷 처리
            if (BytesRead > 0)
            {
                ProcessReceivedPacket(Data, BytesRead, FromAddr);
            }
        }
    }
}

void UIpNetDriver::ProcessReceivedPacket(uint8* Data, int32 DataSize, const FInternetAddr& FromAddr)
{
    // 패킷의 출처에 해당하는 Connection 찾기
    UNetConnection* Connection = FindConnectionForAddress(FromAddr);

    if (!Connection)
    {
        // Connection이 없는 경우 - Connectionless 패킷으로 처리
        ProcessConnectionlessPacket(Data, DataSize, FromAddr);
    }
    else
    {
        // 기존 Connection의 패킷 처리
        Connection->ReceivedPacket(Data, DataSize);
    }
}

void UIpNetDriver::ProcessConnectionlessPacket(uint8* Data, int32 DataSize, const FInternetAddr& FromAddr)
{
    // 패킷 핸들러가 있는 경우 먼저 처리 시도
    if (PacketHandler.IsValid())
    {
        if (PacketHandler->Incoming(Data, DataSize, FromAddr))
        {
            // 패킷 핸들러가 패킷을 처리했으면 종료
            return;
        }
    }

    // 그 외의 Connectionless 패킷 처리 (예: 비콘 메시지 등)
    // ...
}
```

```cpp
void UNetConnection::ReceivedPacket(uint8* Data, int32 DataSize)
{
    // 패킷 핸들러가 있는 경우 먼저 처리
    if (Driver->PacketHandler.IsValid())
    {
        const bool bPacketHandlerProcessed = Driver->PacketHandler->Incoming(Data, DataSize);
        if (bPacketHandlerProcessed)
        {
            return;
        }
    }

    ProcessReceivedPacket(Data, DataSize);
}

void UNetConnection::ProcessReceivedPacket(uint8* Data, int32 DataSize)
{
    // 패킷 데이터를 읽기 위한 비트 리더 생성
    FBitReader Reader(Data, DataSize * 8);

    // 컨트롤 메시지인지 확인
    if (IsControlMessage(Reader))
    {
        uint8 MessageType = Reader.ReadBit() ? 1 : 0;
        FInBunch Bunch(this, Reader);

        // World에 컨트롤 메시지 전달
        Driver->World->NotifyControlMessage(this, MessageType, Bunch);
    }
    else
    {
        // 일반 게임플레이 패킷 처리
        ReceivedPacketHandler(Reader);
    }
}
```

```cpp
bool PacketHandler::Incoming(uint8* Data, int32 CountBytes, const FInternetAddr& FromAddr)
{
    return Incoming_Internal(Data, CountBytes, FromAddr);
}

bool PacketHandler::Incoming_Internal(uint8* Data, int32 CountBytes, const FInternetAddr& FromAddr)
{
    FBitReader Reader(Data, CountBytes * 8);

    // 등록된 모든 핸들러 컴포넌트에서 패킷 처리 시도
    for (int32 i = HandlerComponents.Num() - 1; i >= 0; i--)
    {
        UHandlerComponent* Component = HandlerComponents[i];
        
        if (Component->IsValid())
        {
            // Connectionless 패킷 처리 시도
            if (Component->IncomingConnectionless(Reader, FromAddr))
            {
                return true;
            }
        }
    }

    return false;
}
```

```cpp
void UWorld::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch)
{
    switch (MessageType)
    {
        case NMT_Hello:
        {
            // 클라이언트로부터의 초기 연결 요청
            FString VersionString;
            Bunch << VersionString;

            // 버전 검증 및 연결 승인/거부 처리
            // ...
            break;
        }
        
        case NMT_Challenge:
        {
            // 클라이언트로부터의 챌린지 응답
            uint32 ChallengeResponse;
            Bunch << ChallengeResponse;

            // 챌린지 검증 및 연결 완료 처리
            // ...
            break;
        }

        case NMT_BeaconWelcome:
        {
            // 비콘 관련 처리
            // ...
            break;
        }

        // 기타 컨트롤 메시지 처리
        // ...
    }
}
```

---

## Tip) PlantUML 코드

```
@startuml
skinparam responseMessageBelowArrow true

participant "Client\nUIpNetDriver" as ClientDriver
participant "Client\nPacketHandler" as ClientHandler
participant "Client\nStatelessConnectHandler" as ClientStateless
participant "Server\nUIpNetDriver" as ServerDriver
participant "Server\nPacketHandler" as ServerHandler
participant "Server\nStatelessConnectHandler" as ServerStateless

ClientDriver -> ClientDriver: TickDispatch()
activate ClientDriver

ClientDriver -> ServerDriver: NMT_Hello 전송
deactivate ClientDriver

ServerDriver -> ServerDriver: ProcessConnectionlessPacket()
activate ServerDriver

ServerDriver -> ServerHandler: Incoming()
activate ServerHandler

ServerHandler -> ServerStateless: IncomingConnectionless()
activate ServerStateless
note right: NMT_Hello 수신 처리

ServerStateless -> ServerDriver: NMT_Challenge 생성 및 전송
deactivate ServerStateless
deactivate ServerHandler
deactivate ServerDriver

ClientDriver -> ClientDriver: ProcessConnectionlessPacket()
activate ClientDriver

ClientDriver -> ClientHandler: Incoming()
activate ClientHandler

ClientHandler -> ClientStateless: IncomingConnectionless()
activate ClientStateless
note right: NMT_Challenge 수신 처리

ClientStateless -> ClientStateless: ProcessChallengeResponse()
note right: Challenge 데이터 검증

ClientStateless -> ServerDriver: NMT_ChallengeResponse 전송
deactivate ClientStateless
deactivate ClientHandler
deactivate ClientDriver

ServerDriver -> ServerDriver: ProcessConnectionlessPacket()
activate ServerDriver

ServerDriver -> ServerHandler: Incoming()
activate ServerHandler

ServerHandler -> ServerStateless: IncomingConnectionless()
activate ServerStateless
note right: NMT_ChallengeResponse 검증

alt 검증 성공
    ServerStateless -> ServerDriver: NMT_Welcome 전송
    note right: 연결 승인
else 검증 실패
    ServerStateless -> ServerDriver: 연결 거부
end

@enduml
```
