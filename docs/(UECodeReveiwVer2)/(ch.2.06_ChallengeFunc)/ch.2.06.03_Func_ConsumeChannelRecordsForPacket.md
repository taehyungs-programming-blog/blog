---
layout: default
title: "06-03. [Func] ConsumeChannelRecordsForPacket"
parent: "(06. Challenge과정에서 사용되는 주요 함수)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 목적
    * 패킷 전송 시 어떤 채널들의 데이터가 포함되었는지 기록
    * ACK 수신 시 해당 패킷에 포함된 채널들을 찾아서
    * 각 채널의 번치들 중 해당 패킷 ID를 가진 것들을 처리(Consume)
    * 이런 방식으로 UDP 기반 통신에서 신뢰성 있는 데이터 전송을 구현

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_6_3.png"/>
</p>

```cpp
void UNetConnection::ReceivedPacket(FBitReader& Reader)
{
    FChannelsToClose ChannelsToClose;
    
    // 패킷 헤더 읽기
    FNetPacketNotify::FNotificationHeader Header;
    if (!PacketNotify.ReadHeader(Header, Reader))
    {
        return;
    }

    // 시퀀스 델타 계산
    const int32 PacketSequenceDelta = PacketNotify.GetSequenceDelta(Header);
    if (PacketSequenceDelta > 0)
    {
        // ACK 처리를 위한 람다
        auto HandlePacketNotification = [&Header, &ChannelsToClose, this]
            (FNetPacketNotify::SequenceNumberT AckedSequence, bool bDelivered)
        {
            ++LastNotifiedPacketId;
            
            if (bDelivered)
            {
                ReceivedAck(LastNotifiedPacketId, ChannelsToClose);
            }
            else 
            {
                ReceivedNak(LastNotifiedPacketId);
            }
        };

        // 시퀀스 업데이트 및 패킷 알림 처리
        PacketNotify.Update(Header, HandlePacketNotification);
    }

    // 패킷 디스패치
    bool bSkipAck = false;
    bool bHasBunchErrors = false;
    DispatchPacket(Reader, bSkipAck, bHasBunchErrors);
}
```

```cpp
void UNetConnection::ReceivedAck(int32 AckPacketId, FChannelsToClose& OutChannelsToClose)
{
    // OutAckPacketId 업데이트
    OutAckPacketId = AckPacketId;

    // 채널별 ACK 처리를 위한 람다
    auto AckChannelFunc = [this, &OutChannelsToClose](int32 AckedPacketId, uint32 ChannelIndex)
    {
        UChannel* const Channel = Channels[ChannelIndex];
        if (Channel)
        {
            // 채널의 OutBunch들 처리
            for (FOutBunch* OutBunch = Channel->OutRec; OutBunch; OutBunch = OutBunch->Next)
            {
                if (OutBunch->bOpen)
                {
                    Channel->OpenAcked = 0;
                }

                if (OutBunch->PacketId == AckedPacketId)
                {
                    OutBunch->ReceiveAck = 1;
                }
            }

            // ACK된 번치들 처리
            EChannelCloseReason CloseReason;
            if (Channel->ReceivedAcks(CloseReason))
            {
                OutChannelsToClose.Emplace(FChannelCloseInfo{ChannelIndex, CloseReason});
            }
        }
    };

    // 패킷 ID에 해당하는 모든 채널 기록 처리
    FChannelRecordImpl::ConsumeChannelRecordsForPacket(ChannelRecord, AckPacketId, AckChannelFunc);
}
```

```cpp
template<class Functor>
static void ConsumeChannelRecordsForPacket(FWrittenChannelsRecord& WrittenChannelsRecord, 
    int32 PacketId, Functor&& Func)
{
    FWrittenChannelsRecord::FChannelRecordEntryQueue& Record = WrittenChannelsRecord.ChannelRecord;

    // 첫 번째 엔트리는 패킷 ID여야 함
    FWrittenChannelsRecord::FChannelRecordEntry PacketEntry = Record.Peek();
    Record.Pop();
    check(PacketEntry.IsSequence == 1u && PacketEntry.Value == (uint32)PacketId);

    // 채널 엔트리 처리
    uint32 PreviousChannelIndex = uint32(-1);
    while (!Record.IsEmpty() && Record.PeekNoCheck().IsSequence == 0u)
    {
        const FWrittenChannelsRecord::FChannelRecordEntry Entry = Record.PeekNoCheck();
        Record.PopNoCheck();

        const uint32 ChannelIndex = Entry.Value;

        // 채널당 한 번만 처리
        if (ChannelIndex != PreviousChannelIndex)
        {
            Func(PacketId, ChannelIndex);
            PreviousChannelIndex = ChannelIndex;
        }
    }
}
```


---

## plantuml

```
@startuml
participant "UNetConnection" as conn
participant "FChannelRecordImpl" as impl
participant "UChannel" as channel

[-> conn: ReceivedPacket(Reader)
activate conn

conn -> conn: ReceivedAck(AckPacketId)
activate conn

conn -> impl: ConsumeChannelRecordsForPacket(WrittenChannelsRecord, AckPacketId, AckChannelFunc)
activate impl

loop !Record.IsEmpty() && Record.PeekNoCheck().IsSequence == 0
    impl -> impl: Process Channel Entry
    
    alt ChannelIndex != PreviousChannelIndex
        impl -> channel: AckChannelFunc(PacketId, ChannelIndex)
        activate channel
        
        channel -> channel: ReceivedAcks()
        note right: Process acknowledged bunches\nand potentially close channel
        
        channel --> impl
        deactivate channel
    end
end

impl --> conn
deactivate impl

conn --> conn
deactivate conn

[<-- conn
deactivate conn

@enduml
```