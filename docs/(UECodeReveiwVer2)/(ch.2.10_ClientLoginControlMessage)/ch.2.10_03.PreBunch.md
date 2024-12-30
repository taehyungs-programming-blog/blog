---
layout: default
title: "10-03. PreBunch"
parent: "([Network] 10. ClientLoginControlMessage)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Channel과 Bunch를 먼저 알아야 함

```cpp
// Channel: 특정 유형의 네트워크 통신을 담당하는 클래스
class UChannel 
{
    UNetConnection* Connection;  // 소유 연결
    FOutBunch* OutRec;          // 발신 번치 목록
    FInBunch* InRec;           // 수신 번치 목록
    int32 ChIndex;             // 채널 인덱스
};

// Bunch: 네트워크로 전송되는 데이터 패킷
class FOutBunch 
{
    bool bReliable;            // 신뢰성 여부
    bool bOpen;                // 채널 오픈 번치 여부
    bool bClose;               // 채널 종료 번치 여부
    int32 ChIndex;            // 소속 채널 인덱스
    int32 ChSequence;         // 시퀀스 번호
    FOutBunch* Next;          // 다음 번치 포인터
};
```

```cpp
// 채널을 통한 데이터 전송 예시
void Example_SendDataThroughChannel()
{
    // 1. 채널 생성
    UControlChannel* ControlChannel = Connection->CreateChannel(CHTYPE_Control);
    
    // 2. 번치 생성
    FOutBunch* Bunch = new FOutBunch(ControlChannel);
    Bunch->bReliable = true;
    
    // 3. 번치에 데이터 쓰기
    Bunch->WriteString(TEXT("Hello"));
    Bunch->WriteByte(42);
    
    // 4. 번치 전송
    ControlChannel->SendBunch(Bunch, false);
}
```

---

```cpp
// Channel.cpp

FOutBunch* PrepBunch(FOutBunch* Bunch, FOutBunch* OutBunch, bool Merge)
{
    if (Bunch->bReliable)
    {
    
        if (OutBunch == NULL)
        {

            Bunch->Next = NULL;
            Bunch->ChSequence = ++Connection->OutReliable[ChIndex]; 
            NumOutRec++;
            
            OutBunch = new FOutBunch(*Bunch);
            FOutBunch** OutLink = &OutRec;

            while(*OutLink)
            {
                OutLink = &(*OutLink)->Next;
            }
            *OutLink = OutBunch;
        }
        else
        {
            // @todo
        }
        Connection->LastOutBunch = OutBunch;
    }
    else
    {
        // @todo
    }
    return OutBunch;
}
```

* PrepBunch는 채널에서 발송할 번치(Bunch)를 준비하는 중요한 함수입니다. 주요 기능을 설명하면 다음과 같습니다

* 주요 역할:
    * 발송할 번치를 채널의 신뢰성 있는 전송 시스템에 등록
    * 번치의 시퀀스 번호 할당
    * 채널의 OutRec 리스트에 번치를 연결

* 주요 특징:
    * 신뢰성 있는(Reliable) 번치만 처리
    * 각 채널은 자체 시퀀스 번호 관리
    * OutRec 리스트를 통해 발송된 번치들을 추적
    * 번치의 순서 보장을 위한 시퀀스 관리

* 사용 시점:
    * SendBunch() 함수에서 호출
    * 새로운 번치를 네트워크로 전송하기 전 준비 단계에서 사용

* PrepBunch는 Unreal Engine의 네트워크 시스템에서 신뢰성 있는 데이터 전송을 보장하기 위한 중요한 준비 단계를 담당하는 함수입니다.

