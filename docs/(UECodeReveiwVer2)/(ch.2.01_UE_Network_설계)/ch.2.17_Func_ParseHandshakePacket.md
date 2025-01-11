---
layout: default
title: "02-17. [Func] ParseHandshakePacket"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_1_6.png"/>
</p>

```cpp
virtual void IncomingConnectionless(FIncomingPacketRef PacketRef) override
{
    // 패킷에서 HandshakePacket 여부 확인
    bool bHandshakePacket = !!Packet.ReadBit() && !Packet.IsError();
    if (bHandshakePacket)
    {
        FParsedHandshakeData HandshakeData;
        bHandshakePacket = ParseHandshakePacket(Packet, HandshakeData);
        // ...
    }
}
```

---

## Tip) PlantUML

```
@startuml
skinparam responseMessageBelowArrow true

participant "UIpNetDriver" as Driver
participant "StatelessConnectHandlerComponent" as Handler
participant "FBitReader" as Reader

== 서버 측 Initial Packet 처리 ==
Driver -> Handler: IncomingConnectionless()
activate Handler
Handler -> Reader: ReadBit() // bHandshakePacket 확인
Handler -> Handler: ParseHandshakePacket()
note right: Initial Packet 파싱
Handler -> Handler: SendConnectChallenge()
deactivate Handler

== 클라이언트 측 Challenge 처리 ==
Driver -> Handler: Incoming()
activate Handler
Handler -> Reader: ReadBit() // bHandshakePacket 확인 
Handler -> Handler: ParseHandshakePacket()
note right: Challenge Packet 파싱
Handler -> Handler: SendChallengeResponse()
deactivate Handler

== 서버 측 Response 처리 ==
Driver -> Handler: IncomingConnectionless()
activate Handler
Handler -> Reader: ReadBit() // bHandshakePacket 확인
Handler -> Handler: ParseHandshakePacket()
note right: Response Packet 파싱
Handler -> Handler: SendChallengeAck()
deactivate Handler

== 클라이언트 측 Ack 처리 ==
Driver -> Handler: Incoming()
activate Handler
Handler -> Reader: ReadBit() // bHandshakePacket 확인
Handler -> Handler: ParseHandshakePacket()
note right: Ack Packet 파싱
Handler -> Handler: Initialized()
deactivate Handler

@enduml
```