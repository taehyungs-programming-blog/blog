---
layout: default
title: "([Network] 02. NMT_Hello)"
parent: "(UnrealEngine Code-Review Ver.2)"
has_children: true
nav_order: 2
---

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_2_1.png"/>
</p>

```cpp
void UWorld::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch)
{
    if (NetDriver->ServerConnection)
    {
        // 클라이언트 로직...
    }
    else // 서버 로직
    {
        switch (MessageType)
        {
            case NMT_Hello:
            {
                uint8 IsLittleEndian = 0;
                uint32 RemoteNetworkVersion = 0;
                uint32 LocalNetworkVersion = FNetworkVersion::GetLocalNetworkVersion();
                FString EncryptionToken;
                
                // Hello 메시지 역직렬화
                if (FNetControlMessage<NMT_Hello>::Receive(Bunch, 
                    IsLittleEndian, 
                    RemoteNetworkVersion, 
                    EncryptionToken,
                    RemoteNetworkFeatures))
                {
                    if (EncryptionToken.IsEmpty())
                    {
                        // Challenge 메시지 전송
                        Connection->SendChallengeControlMessage();
                    }
                    else
                    {
                        // 암호화 관련 처리...
                    }
                }
                break;
            }
            // 다른 메시지 타입 처리...
        }
    }
}
```

```cpp
template<>
struct FNetControlMessage<NMT_Hello>
{
    static bool Receive(FInBunch& Bunch, 
        uint8& IsLittleEndian,
        uint32& RemoteNetworkVersion,
        FString& EncryptionToken,
        FNetworkFeatures& RemoteNetworkFeatures)
    {
        // 메시지 역직렬화
        bool bRead = Bunch.ReadBit(&IsLittleEndian);
        bRead &= Bunch.SerializeIntPacked(RemoteNetworkVersion);
        bRead &= Bunch.ReadString(EncryptionToken);
        
        if (RemoteNetworkVersion >= HISTORY_MULTIPLE_NETWORKING_FEATURES)
        {
            bRead &= RemoteNetworkFeatures.Serialize(Bunch);
        }
        
        return bRead;
    }
};
```

```cpp
void UNetConnection::SendChallengeControlMessage()
{
    // 챌린지 데이터 생성
    uint32 ChallengeValue = FMath::Rand();
    
    // 서버의 네트워크 버전 정보
    const uint32 LocalNetworkVersion = FNetworkVersion::GetLocalNetworkVersion();
    
    // Challenge 메시지 전송
    FNetControlMessage<NMT_Challenge>::Send(this, 
        ChallengeValue,
        LocalNetworkVersion);
    
    // 연결 상태 업데이트
    SetClientLoginState(EClientLoginState::Challenge);
    
    // Challenge 값 저장
    SetExpectedClientLoginMsgType(NMT_Login);
    ServerChallengeValue = ChallengeValue;
    
    // 즉시 전송
    FlushNet();
}
```

```cpp
template<>
struct FNetControlMessage<NMT_Challenge>
{
    static bool Send(UNetConnection* Conn, 
        uint32 Challenge,
        uint32 NetworkVersion)
    {
        FNetBitWriter Writer(0);
        
        // Challenge 값과 네트워크 버전 직렬화
        Writer.SerializeIntPacked(Challenge);
        Writer.SerializeIntPacked(NetworkVersion);
        
        // 메시지 전송
        return Conn->SendRawMessage(Writer, NMT_Challenge);
    }
};
```

---

## Tip) PlantUML Code

```
@startuml
participant Client
participant "Server\nUWorld" as World
participant "UNetConnection" as Connection

Client -> World: NMT_Hello\n(IsLittleEndian, RemoteNetworkVersion, EncryptionToken)
activate World

World -> World: NotifyControlMessage(NMT_Hello)
activate World #DarkSalmon

World -> World: FNetControlMessage<NMT_Hello>::Receive()
note right: 메시지 파라미터 역직렬화

alt EncryptionToken이 비어있는 경우
    World -> Connection: SendChallengeControlMessage()
    activate Connection
    Connection --> Client: NMT_Challenge
    deactivate Connection
else EncryptionToken이 있는 경우
    note right: 암호화 관련 처리
end

deactivate World
deactivate World

@enduml
```