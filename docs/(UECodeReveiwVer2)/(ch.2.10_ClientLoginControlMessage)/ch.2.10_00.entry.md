---
layout: default
title: "([Network] 10. ClientLoginControlMessage)"
parent: "(UnrealEngine Code-Review Ver.2)"
has_children: true
nav_order: 4
---

* 시작은 `FNetPacketNotify`의 `Update()`에서 시작된다.

---

* 그런데 잠깐! `FNetPacketNotify`는 뭐하는 클래스였지?

## FNetPacketNotify

* `NetPacketNotify`는 신뢰성 있는 UDP 통신을 구현하기 위한 핵심 클래스입니다. 주요 목적은:
  * 패킷의 시퀀스 번호 관리
  * ACK/NAK 처리를 통한 패킷 전달 보장
  * 패킷 손실 감지 및 재전송 관리

```cpp
// 간단한 NetPacketNotify 사용 예시
class SimpleNetPacketNotify {
    using SequenceNumberT = uint16;
    static const int HistorySize = 256;  // 256비트 히스토리

    struct PacketHistory {
        std::bitset<HistorySize> deliveryStatus;  // 각 비트가 패킷 전달 상태 표시
        void AddDeliveryStatus(bool delivered) {
            // 비트를 왼쪽으로 시프트하고 새로운 상태 추가
            deliveryStatus <<= 1;
            deliveryStatus.set(0, delivered);
        }
    };

    // 송신측 상태
    SequenceNumberT outSeq = 0;        // 보내는 패킷 번호
    SequenceNumberT outAckSeq = 0;     // 수신 확인된 패킷 번호

    // 수신측 상태
    SequenceNumberT inSeq = 0;         // 받은 패킷 번호
    SequenceNumberT inAckSeq = 0;      // ACK 보낸 패킷 번호
    PacketHistory inSeqHistory;        // 수신 패킷 히스토리

public:
    // 패킷 전송 시 호출
    void SendPacket(NetworkPacket& packet) {
        // 패킷에 시퀀스 번호 추가
        packet.SetSequenceNumber(outSeq);
        // 마지막으로 받은 패킷 번호와 히스토리 추가
        packet.SetAckData(inAckSeq, inSeqHistory);
        
        outSeq++;
    }

    // 패킷 수신 시 호출
    bool ReceivePacket(const NetworkPacket& packet) {
        SequenceNumberT receivedSeq = packet.GetSequenceNumber();
        
        // 1. 수신된 패킷이 예상 순서인지 확인
        if (IsValidSequence(receivedSeq)) {
            // 패킷 수신 처리
            while (inSeq < receivedSeq) {
                // 중간에 손실된 패킷은 NAK로 표시
                inSeqHistory.AddDeliveryStatus(false);
                inSeq++;
            }
            
            // 현재 패킷은 성공적으로 수신
            inSeqHistory.AddDeliveryStatus(true);
            inSeq = receivedSeq + 1;
            inAckSeq = receivedSeq;

            // 2. 상대방이 보낸 ACK 정보 처리
            ProcessReceivedAcks(packet.GetAckSeq(), packet.GetAckHistory());
            
            return true;
        }
        
        return false;
    }

private:
    bool IsValidSequence(SequenceNumberT seq) {
        // 시퀀스 번호가 유효한 범위인지 확인
        const int32_t diff = seq - inSeq;
        return (diff >= 0 && diff < HistorySize);
    }

    void ProcessReceivedAcks(SequenceNumberT ackSeq, const PacketHistory& history) {
        // 수신측이 확인한 패킷 처리
        while (outAckSeq < ackSeq) {
            outAckSeq++;
            if (history.deliveryStatus[outAckSeq % HistorySize]) {
                // ACK: 패킷이 성공적으로 전달됨
                OnPacketAcked(outAckSeq);
            } else {
                // NAK: 패킷 손실, 재전송 필요
                OnPacketNaked(outAckSeq);
            }
        }
    }

    void OnPacketAcked(SequenceNumberT seq) {
        // ACK된 패킷 처리 (예: 버퍼에서 제거)
    }

    void OnPacketNaked(SequenceNumberT seq) {
        // NAK된 패킷 처리 (예: 재전송 큐에 추가)
    }
};

// 사용 예시
void NetworkManager::Update() {
    SimpleNetPacketNotify packetNotify;
    
    // 패킷 전송
    NetworkPacket outPacket;
    packetNotify.SendPacket(outPacket);
    network.Send(outPacket);

    // 패킷 수신
    NetworkPacket inPacket;
    if (network.Receive(inPacket)) {
        if (packetNotify.ReceivePacket(inPacket)) {
            // 패킷 처리 성공
            ProcessPacketData(inPacket);
        }
    }
}
```

* 이 예시 코드는 다음과 같은 `NetPacketNotify`의 핵심 기능을 보여줍니다:
  * 시퀀스 번호 관리:
    * `outSeq`: 송신 패킷 번호
    * `inSeq`: 수신 패킷 번호
  * ACK/NAK 처리:
    * `inSeqHistory`: 수신된 패킷의 전달 상태 기록
    * `ProcessReceivedAcks()`: ACK/NAK 정보 처리
  * 패킷 유효성 검사:
    * `IsValidSequence()`: 수신된 패킷의 시퀀스 번호가 유효한지 확인
  * 히스토리 관리:
    * `PacketHistory`: 비트셋을 사용한 패킷 전달 상태 기록
    * `AddDeliveryStatus()`: 새로운 전달 상태 추가

---

## 그럼 `FNetPacketNotify`의 `Update()`는 어디서 호출될까?

```
UNetConnection::ReceivedPacket() 
    -> UNetConnection::ReceivedRawPacket()
        -> UNetConnection::HandlePacketNotification() // Update의 콜백 함수로 전달됨
            -> FNetPacketNotify::Update()
```

```
UNetConnection::ReceivedPacket()
    -> UNetConnection::ReceivedRawPacket() 
        -> UNetConnection::HandlePacketNotification() // Update의 콜백 함수로 전달됨
            -> FNetPacketNotify::Update()
```

---

## 시작

```cpp
// FNetPacketNotify.h

template<class Functor>
SequenceNumberT::DifferenceT Update(const FNotificationHeader& NotificationData, Functor&& InFunc)
{
    const SequenceNumberT::DifferenceT InSeqDelta = GetSequenceDelta(NotificationData);
    if (InSeqDelta > 0)
    {
        ProcessReceivedAcks(NotificationData, InFunc);

        // ...

        return InternalUpdate(NotificationData, InSeqDelta);
    }
}
```