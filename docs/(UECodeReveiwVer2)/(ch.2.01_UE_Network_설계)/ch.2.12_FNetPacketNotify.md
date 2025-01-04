---
layout: default
title: "02-12. FNetPacketNotify"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* FNetPacketNotify는 언리얼 엔진의 신뢰성 있는 UDP 통신을 구현하기 위한 핵심 클래스
* 시퀀스 번호 관리
    * 들어오는 패킷(InSeq)과 나가는 패킷(OutSeq)의 시퀀스 번호를 추적
    * 14비트 시퀀스 번호 사용 (0-16383 범위)
* 패킷 수신 확인 (Acknowledgment)
    * 받은 패킷의 기록 유지 (InSeqHistory)
    * 마지막으로 확인된 수신/발신 시퀀스 추적 (InAckSeq, OutAckSeq)

```cpp
// NetConnection.h
class UNetConnection : public UPlayer
{
    // ... 다른 코드들 ...

    /** Packet notification tracking */
    FNetPacketNotify PacketNotify;
    
    /** Processes received packets and dispatches them to the proper subsystems */
    virtual void ReceivedPacket(FBitReader& Reader);
    
    /** Sends a packet */
    virtual void SendPacket();
    
    // ... 다른 코드들 ...
};
```

```cpp
void UNetConnection::ReceivedPacket(FBitReader& Reader)
{
    // 패킷에서 시퀀스 번호 읽기
    uint32 SeqNum = Reader.ReadBits(PacketNotify.SequenceNumberBits);
    
    // PacketNotify를 통해 시퀀스 번호 처리
    if (PacketNotify.InSeq.Value < SeqNum)
    {
        // 새로운 패킷 처리
        PacketNotify.InSeq.Value = SeqNum;
        PacketNotify.InSeqHistory.MarkReceived(SeqNum);
    }
    
    // ... 패킷 데이터 처리 ...
}
```

```cpp
// 패킷 송신 측 예시
void SendReliablePacket(FNetPacketNotify& PacketNotify, const FPacket& Packet)
{
    // 새로운 시퀀스 번호 할당
    auto SequenceNumber = PacketNotify.OutSeq.Value;
    
    // 패킷에 시퀀스 번호 포함하여 전송
    Packet.SetSequenceNumber(SequenceNumber);
    Network.Send(Packet);
    
    // 다음 시퀀스 번호로 증가
    PacketNotify.OutSeq.Value = (SequenceNumber + 1) & PacketNotify.SequenceNumberT::SeqNumberMask;
}

// 패킷 수신 측 예시
void ReceivePacket(FNetPacketNotify& PacketNotify, const FPacket& Packet)
{
    auto ReceivedSeq = Packet.GetSequenceNumber();
    
    // 시퀀스 번호가 유효한지 확인
    if (IsValidSequence(ReceivedSeq))
    {
        // 수신 기록 업데이트
        PacketNotify.InSeqHistory.MarkReceived(ReceivedSeq);
        PacketNotify.InSeq.Value = ReceivedSeq;
        
        // ACK 전송
        SendAcknowledgement(ReceivedSeq);
    }
}
```