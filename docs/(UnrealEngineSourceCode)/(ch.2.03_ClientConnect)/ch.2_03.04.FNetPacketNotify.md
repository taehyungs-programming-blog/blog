---
layout: default
title: "03-04. FNetPacketNotify"
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
/** 수신된 시퀀스 번호의 기록을 관리하는 유틸리티 클래스 */
template <SIZE_T HistorySize>
class TSequenceHistory
{
    // @todo
    // @할일
};

/**
 * drives delivery of sequence numbers, acknowledgements and notifications of delivery sequence numbers
 */
/**
 * 시퀀스 번호의 전달, 확인 응답 및 전달 시퀀스 번호의 알림을 처리합니다
 */

// this is one of key structure to impelment the reliable UDP on each NetConnection
// - let's see briefly member variables
// - we'll see how it works in details in near future, looking into how Actor replication works

// 이것은 각 NetConnection에서 신뢰할 수 있는 UDP를 구현하기 위한 주요 구조 중 하나입니다
// - 멤버 변수들을 간단히 살펴보겠습니다
// - 앞으로 Actor 복제가 어떻게 작동하는지 살펴보면서 자세히 알아볼 것입니다
class FNetPacketNotify
{
public:
    enum { SequenceNumberBits = 14 };
    enum { MaxSequenceHistoryLength = 256 };

    typedef TSequenceNumber<SequenceNumberBits, uint16> SequenceNumberT;
    typedef TSequenceHistory<MaxSequenceHistoryLength> SequenceHistoryT;

    /** init notification with expected initial sequence numbers */
    /** 예상되는 초기 시퀀스 번호로 알림을 초기화합니다 */
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
    /** 수신된 패킷의 기록을 설명하는 비트필드를 포함하는 비트 버퍼 */
    SequenceHistoryT InSeqHistory;

    /** last sequence number received and accepted from remote */
    /** 원격에서 수신하고 수락한 마지막 시퀀스 번호 */
    SequenceNumberT InSeq;
    /** last sequence number received from remote that we have acknowledged, this is needed since we support accepting a packet but explicitly not acknowledge it as received */
    /** 원격에서 수신하고 우리가 확인 응답한 마지막 시퀀스 번호, 패킷을 수락하지만 명시적으로 수신을 확인하지 않는 것을 지원하기 때문에 필요합니다 */
    SequenceNumberT InAckSeq;
    /** last sequence number received from remote that we have acknowledged and also knows that the remote has received the ack, used to calculate how big our history must be */
    /** 원격에서 수신하고 우리가 확인 응답했으며, 원격이 확인 응답을 받았다는 것을 알고 있는 마지막 시퀀스 번호, 우리의 기록이 얼마나 커야 하는지 계산하는 데 사용됩니다 */
    SequenceNumberT InAckSeqAck;
    SequenceNumberT WaitingForFlushSeqAck;

    /**
     * tracking outoging sequence data
     */
    /**
     * 송신 시퀀스 데이터 추적
     */

    /** outgoing sequence number */
    /** 송신 시퀀스 번호 */
    SequenceNumberT OutSeq;
    /** last sequence number that we know that the remote side have received */
    /** 원격 측에서 수신한 것으로 알고 있는 마지막 시퀀스 번호 */
    SequenceNumberT OutAckSeq;
};
```