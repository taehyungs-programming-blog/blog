---
layout: default
title: "02-05. FOutBunch"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review 2025 Ver. 🐍)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
class FOutBunch : public FNetBitWriter
{
public:
    // Bunch 헤더 정보
    struct 
    {
        uint8 bControl:1;        // 컨트롤 메시지인지 여부
        uint8 bReliable:1;       // 신뢰성 있는 전송인지 여부
        uint8 bOpen:1;           // 채널 오픈 메시지인지 여부
        uint8 bClose:1;          // 채널 클로즈 메시지인지 여부
        uint8 bDormant:1;        // 도먼트 상태 메시지인지 여부
        uint8 bIsReplicationPaused:1;  // 리플리케이션 일시정지 상태인지
        uint8 bPartial:1;        // 분할된 메시지인지 여부
        uint8 bPartialInitial:1; // 분할된 메시지의 시작인지
        uint8 bPartialFinal:1;   // 분할된 메시지의 끝인지
    } PacketFlags;

    int32 ChIndex;      // 채널 인덱스
    int32 ChSequence;   // 채널 시퀀스 번호
    int32 PacketId;     // 패킷 ID
    
    UChannel* Channel;   // 소속 채널
    double Time;        // 타임스탬프
};
```

```cpp
void UChannel::SendData()
{
    // Bunch 생성
    FOutBunch* Bunch = new FOutBunch(this);
    
    // 신뢰성 설정
    Bunch->bReliable = true;
    
    // 데이터 직렬화
    Bunch->WriteInt(SomeData);
    Bunch->WriteString(SomeString);
    
    // 전송
    Connection->SendBunch(Bunch);
}
```