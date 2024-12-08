---
layout: default
title: "10-02. ConsumeChannelRecordsForPacket를 어떻게 쓰나?"
parent: "([Network] 10. ClientLoginControlMessage)"
grand_parent: "(UnrealEngine Code-Review 2025 Ver. 🐍)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 각 채널별로 해당 패킷에 포함된 번치(Bunch)들을 처리합니다

```cpp
    auto AckChannelFunc = [this, &OutChannelsToClose](int32 AckedPacketId, uint32 ChannelIndex)
    {

        UChannel* const Channel = Channels[ChannelIndex];
        if (Channel)
        {
                                                                                                    
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

            EChannelCloseReason CloseReason;
            if (Channel->ReceivedAcks(CloseReason))
            {
                const FChannelCloseInfo Info = {ChannelIndex, CloseReason};
                OutChannelsToClose.Emplace(Info);
            }
        }
    };

    FChannelRecordImpl::ConsumeChannelRecordsForPacket(ChannelRecord, AckPacketId, AckChannelFunc);
}
```

* bOpen 번치가 있으면 OpenAcked를 0으로 설정 (채널 열기 확인은 별도로 처리)
* 패킷 ID가 일치하는 번치의 ReceiveAck를 1로 설정 (성공적으로 전달됨을 표시)

```cpp
// 채널정리
EChannelCloseReason CloseReason;
if (Channel->ReceivedAcks(CloseReason))
{
    const FChannelCloseInfo Info = {ChannelIndex, CloseReason};
    OutChannelsToClose.Emplace(Info);
}
```

```
[패킷 수신] -> [채널별 번치 확인] -> [ACK 표시] -> [채널 정리]
```