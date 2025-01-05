---
layout: default
title: "([Network] 05. BeginHandShake 과정 정리)"
parent: "(UnrealEngine Code-Review Ver.2)"
has_children: true
nav_order: 2
---

```cpp
void UPendingNetGame::InitNetDriver()
{=
    if (GEngine->CreateNamedNetDriver(this, NAME_PendingNetDriver, NAME_GameNetDriver))
    {
        NetDriver = GEngine->FindNamedNetDriver(this, NAME_PendingNetDriver);
    }

    if (NetDriver->InitConnect(this, URL, ConnectionError))
    {
        ULocalPlayer* LocalPlayer = GEngine->GetFirstGamePlayer(this);
        if (LocalPlayer)
        {
            // 여기서 부터 시작된다.
            LocalPlayer->PreBeginHandshake(ULocakPlayer::FOnPreBeginHandshakeCompleteDelegate::CreateWeakLambda(this,
                [this]()
                {
                    BeginHandshake();
                }));
        }
    }
}
```

```cpp
void UPendingNetGame::BeginHandshake()
{
    UNetConnection* ServerConn = NetDriver->ServerConnection;
    if (ServerConn->Handler.IsValid())
    {
        ServerConn->Handler->BeginHandshaking(
            FPacketHandlerHandshakeComplete::CreateUObject(this, &UPendingNetGame::SendInitialJoin)
        );
    }
}
```

---

## UML

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_5_1.png"/>
</p>

---

## Tip) plantuml 코드

```
@startuml
participant UPendingNetGame
participant UNetConnection as "ServerConnection"
participant PacketHandler
participant "StatelessConnectHandlerComponent" as HandlerComponent

UPendingNetGame -> UPendingNetGame: BeginHandshake()
activate UPendingNetGame

UPendingNetGame -> PacketHandler: BeginHandshaking(SendInitialJoin)
activate PacketHandler

note right of PacketHandler
  bBeganHandshaking = true
  HandshakeCompleteDel = SendInitialJoin
end note

PacketHandler -> HandlerComponent: NotifyHandshakeBegin()
activate HandlerComponent

note right of HandlerComponent
  Component가 handshake를 필요로 하고
  아직 초기화되지 않은 경우에만 호출
end note

HandlerComponent --> PacketHandler
deactivate HandlerComponent

PacketHandler --> UPendingNetGame
deactivate PacketHandler

note right of UPendingNetGame
  Handshake가 완료되면
  HandshakeCompleteDel(SendInitialJoin)이
  나중에 호출됨
end note

deactivate UPendingNetGame
@enduml
```