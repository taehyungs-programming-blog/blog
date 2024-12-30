---
layout: default
title: "10-01. ConsumeChannelRecordsForPacket"
parent: "([Network] 10. ClientLoginControlMessage)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 우선 NetConnection 클래스를 먼저 알아야 함

* NetConnection은 클라이언트와 서버 간의 네트워크 연결을 관리하는 핵심 클래스입니다.

* 주요 목적:
    * 네트워크 연결 상태 관리
    * 패킷의 송수신 처리
    * 채널 관리 (Control, Actor, Voice 등)
    * 신뢰성 있는 데이터 전송 보장

```cpp
class UNetConnection : public UPlayer
{
    // 연결 상태
    EConnectionState State;  // Invalid, Closed, Pending, Open
    
    // 채널 관리
    TArray<UChannel*> Channels;        // 모든 채널
    TArray<UChannel*> OpenChannels;    // 활성 채널
    TArray<UChannel*> ChannelsToTick;  // 틱 처리가 필요한 채널
    
    // 패킷 관리
    FBitWriter SendBuffer;             // 전송 버퍼
    int32 OutPacketId;                 // 보낸 패킷 ID
    int32 InPacketId;                  // 받은 패킷 ID
};
```

```cpp
// 서버 측 코드
void AGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    // 새로운 플레이어 접속 시 NetConnection 생성
    UNetConnection* Connection = NewPlayer->GetNetConnection();
    
    // 컨트롤 채널 생성
    UControlChannel* ControlChannel = (UControlChannel*)Connection->CreateChannelByName(
        "Control", EChannelCreateFlags::OpenedLocally);
    
    // 환영 메시지 전송
    FNetControlMessage<NMT_Welcome>::Send(Connection);
    Connection->FlushNet();  // 즉시 전송
}

// 클라이언트 측 코드
void APlayerController::SendMove()
{
    // 이동 데이터 전송
    UNetConnection* Connection = GetNetConnection();
    if (Connection)
    {
        FNetBitWriter Writer;
        SerializeMove(Writer);  // 이동 데이터 직렬화
        
        // Actor 채널을 통해 전송
        UActorChannel* Ch = Connection->FindActorChannel(this);
        if (Ch)
        {
            Ch->SendBunch(Writer, true);  // reliable 전송
        }
    }
}
```

---

## 이제, ConsumeChannelRecordsForPacket 

* 패킷 하나에는 여러 채널의 데이터를 포함할 수 있다.

```cpp
// 패킷 구조 예시
struct Packet {
    int32 PacketId;        // 예: 1000
    TArray<ChannelData> Channels;  // 이 패킷에 포함된 여러 채널의 데이터
};

// 실제 패킷 예시:
Packet 1000 {
    // Control 채널 데이터
    Channel[0] = "로그인 정보"
    
    // Actor 채널 데이터
    Channel[1] = "플레이어 위치 정보"
    
    // Voice 채널 데이터
    Channel[2] = "음성 채팅 데이터"
}
```

```cpp
// 클라이언트에서 서버로 데이터를 보내는 상황
void Example()
{
    // 패킷 1000이 다음 데이터를 포함
    {
        // 컨트롤 채널(0번)을 통해 채팅 메시지 전송
        ControlChannel->SendMessage("안녕하세요");
        
        // 액터 채널(1번)을 통해 캐릭터 이동 정보 전송
        ActorChannel->SendMovement(Position, Rotation);
        
        // 보이스 채널(2번)을 통해 음성 데이터 전송
        VoiceChannel->SendVoiceData(VoicePacket);
    }
}
```

---

* 이 함수는 특정 패킷(PacketId)에 포함된 모든 채널 데이터를 처리하는 함수입니다.

```cpp
// 채널 기록 구조
struct FChannelRecordEntry
{
    uint32 Value : 31;      // PacketId 또는 ChannelIndex
    uint32 IsSequence : 1;  // 1=PacketId, 0=ChannelIndex
};

// 실제 저장 예시:
// [PacketId=1000, IsSeq=1] [ChIndex=5, IsSeq=0] [ChIndex=7, IsSeq=0]
// [PacketId=1001, IsSeq=1] [ChIndex=5, IsSeq=0]
```

```cpp
template<class Functor>
static void ConsumeChannelRecordsForPacket(FWrittenChannelsRecord& WrittenChannelsRecord, 
    int32 PacketId, Functor&& Func)
{
    uint32 PreviousChannelIndex = uint32(-1);
    auto& Record = WrittenChannelsRecord.ChannelRecord;

    // 1. PacketId 엔트리 확인
    FChannelRecordEntry PacketEntry = Record.Peek();
    Record.Pop();
    check(PacketEntry.IsSequence == 1u && PacketEntry.Value == (uint32)PacketId);

    // 2. 연관된 채널들 처리
    while (!Record.IsEmpty() && Record.PeekNoCheck().IsSequence == 0u)
    {
        const FChannelRecordEntry Entry = Record.PeekNoCheck();
        Record.PopNoCheck();
        
        const uint32 ChannelIndex = Entry.Value;
        
        // 각 채널은 패킷당 한 번만 처리
        if (ChannelIndex != PreviousChannelIndex)
        {
            Func(PacketId, ChannelIndex);
            PreviousChannelIndex = ChannelIndex;
        }
    }
}
```

```cpp
// ReceivedAck에서의 사용
void UNetConnection::ReceivedAck(int32 AckPacketId)
{
    // 패킷 확인 응답 처리를 위한 람다 함수
    auto AckChannelFunc = [this](int32 AckedPacketId, uint32 ChannelIndex)
    {
        UChannel* Channel = Channels[ChannelIndex];
        if (Channel)
        {
            // 채널의 번치들 처리
            for (FOutBunch* OutBunch = Channel->OutRec; OutBunch; OutBunch = OutBunch->Next)
            {
                if (OutBunch->PacketId == AckedPacketId)
                {
                    OutBunch->ReceivedAck = true;  // 확인 응답 표시
                }
            }
        }
    };

    // 채널 기록 처리
    ConsumeChannelRecordsForPacket(ChannelRecord, AckPacketId, AckChannelFunc);
}
```

```cpp
// 서버가 패킷 1000을 받았을 때
void ProcessReceivedPacket()
{
    // 패킷 1000에 포함된 모든 채널 데이터 처리
    ConsumeChannelRecordsForPacket(ChannelRecord, 1000, 
        [](int32 PacketId, uint32 ChannelIndex) {
            switch(ChannelIndex) {
                case 0: // 컨트롤 채널
                    ProcessChatMessage();
                    break;
                    
                case 1: // 액터 채널
                    UpdatePlayerPosition();
                    break;
                    
                case 2: // 보이스 채널
                    PlayVoiceData();
                    break;
            }
        });
}
```