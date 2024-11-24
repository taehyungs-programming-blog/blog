---
layout: default
title: "02-03. UChannel"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review 2025 Ver. 🐍)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UChannel 설계 이념

* 데이터 분류와 관리
    * 특정 목적의 데이터 전송을 담당
    * 각 타입별 전용 채널 제공 (Control, Actor, Voice 등)
    * 데이터의 논리적 분리
* Bunch 시스템
    * 데이터를 Bunch 단위로 관리
    * 신뢰성과 순서 보장
    * 분할 및 재조립

```cpp
class UChannel : public UObject
{
protected:
    // 채널 기본 정보
    UNetConnection* Connection;
    EChannelType ChType;
    int32 ChIndex;
    bool bOpenedLocally;
    
    // Bunch 관리
    TArray<FOutBunch*> OutgoingBunches;
    TArray<FInBunch*> IncomingBunches;
    
    // 시퀀스 관리
    int32 OutSequence;
    int32 InSequence;

public:
    // 채널 초기화
    virtual bool Init(UNetConnection* InConnection, int32 InChIndex, bool bInOpenedLocally)
    {
        Connection = InConnection;
        ChIndex = InChIndex;
        bOpenedLocally = bInOpenedLocally;
        
        // 채널 타입별 초기화
        return InitType();
    }

    // Bunch 전송
    virtual FOutBunch* CreateOutgoingBunch(bool bClose, bool bReliable)
    {
        FOutBunch* Bunch = new FOutBunch(this, bClose);
        Bunch->bReliable = bReliable;
        Bunch->ChSequence = ++OutSequence;
        
        if (bReliable)
        {
            OutgoingBunches.Add(Bunch);
        }
        
        return Bunch;
    }

    // Bunch 수신
    virtual void ReceivedBunch(FInBunch& Bunch)
    {
        // 시퀀스 체크
        if (Bunch.bReliable)
        {
            if (Bunch.ChSequence != InSequence + 1)
            {
                // 순서가 맞지 않으면 버퍼링
                BufferBunch(Bunch);
                return;
            }
            InSequence = Bunch.ChSequence;
        }
        
        // 채널 타입별 처리
        ProcessBunch(Bunch);
    }
};
```