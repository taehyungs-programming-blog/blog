---
layout: default
title: "03-07. UNetConnection"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
/** Helper class to work with sequence numbers */
/** 시퀀스 번호를 다루기 위한 헬퍼 클래스 */
template <SIZE_T NumBits, typename SequenceType>
class TSequenceNumber
{
public:
    using SequenceT = SequenceType; 

    enum { SeqNumberBits = NumBits };
    enum { SeqNumberCount = SequenceT(1) << NumBits };
    enum { SeqNumberHalf = SequenceT(1) << (NumBits - 1) };
    enum { SeqNumberMax = SeqNumberCount - 1u };
    enum { SeqNumberMask = SeqNumberMax };

    TSequenceNumber() : Value(0u) {}
    TSequenceNumber(SequenceT ValueIn) : Value(ValueIn & SeqNumberMask) {}

    SequenceT Value;
};

/** Util class to manage history of received sequence numbers */
/** 수신된 시퀀스 번호의 기록을 관리하기 위한 유틸리티 클래스 */
template <SIZE_T HistorySize>
class TSequenceHistory
{
    // @todo
};

/**
 * drives delivery of sequence numbers, acknowledgements and notifications of delivery sequence numbers
 */
/**
 * 시퀀스 번호의 전달, 승인 및 전달 시퀀스 번호의 알림을 처리함
 */

// this is one of key structure to impelment the reliable UDP on each NetConnection
// - let's see briefly member variables
// - we'll see how it works in details in near future, looking into how Actor replication works
// *** see briefly above 'TSequenceNumber'

// 이것은 각 NetConnection에서 신뢰할 수 있는 UDP를 구현하기 위한 핵심 구조 중 하나입니다
// - 멤버 변수들을 간단히 살펴보겠습니다
// - 앞으로 Actor 복제가 어떻게 작동하는지 살펴보면서 자세히 알아볼 예정입니다
// *** 위의 'TSequenceNumber'를 간단히 참고하세요

class FNetPacketNotify
{
public:
    enum { SequenceNumberBits = 14 };
    enum { MaxSequenceHistoryLength = 256 };

    typedef TSequenceNumber<SequenceNumberBits, uint16> SequenceNumberT;
    typedef TSequenceHistory<MaxSequenceHistoryLength> SequenceHistoryT;

    /** init notification with expected initial sequence numbers */
    /** 예상되는 초기 시퀀스 번호로 알림을 초기화 */
    void Init(SequenceNumberT InitialInSeq, SequenceNumberT InitialOutSeq)
    {
        InSeqHistory.Reset();
        InSeq = InitialInSeq;
        InAckSeq = InitialInSeq;
        InAckSeqAck = InitialInSeq;
        OutSeq = InitialOutSeq;
        OutAckSeq = SequenceNumberT(InitialOutSeq.Get() - 1);
        WaitingForFlushSeqAck = OutAckSeq;
    }

    /**
     * tracking incoming sequence data
     */
    /**
     * 수신 시퀀스 데이터 추적
     */

    /** bit buffer containing a bitfield describing the history of received packets */
    /** 수신된 패킷의 기록을 설명하는 비트필드가 포함된 비트 버퍼 */
    SequenceHistoryT InSeqHistory;

    /** last sequence number received and accepted from remote */
    /** 원격에서 수신하고 승인한 마지막 시퀀스 번호 */
    SequenceNumberT InSeq;
    
    /** last sequence number received from remote that we have acknowledged, this is needed since we support accepting a packet but explicitly not acknowledge it as received */
    /** 원격에서 수신하여 승인한 마지막 시퀀스 번호, 패킷을 수신은 했지만 명시적으로 수신 확인을 하지 않을 수 있기 때문에 필요함 */
    SequenceNumberT InAckSeq;
    
    /** last sequence number received from remote that we have acknowledged and also knows that the remote has received the ack, used to calculate how big our history must be */
    /** 원격에서 수신하여 승인했고, 원격에서도 승인을 받았다는 것을 알고 있는 마지막 시퀀스 번호, 기록의 크기를 계산하는 데 사용됨 */
    SequenceNumberT InAckSeqAck;
```
