---
layout: default
title: "06. Unreal Insight + Replication 빈도 및 연관성"
parent: "(Unreal Network)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/6)

## Unreal Insight 실행

* **UnrealInsights.exe** 실행
* (option) PATH에 UnrealEditor.exe가 있는 폴더를 추가해 아래 명령이 바로 실행되게 한다.

```bash
$ UnrealEditor.exe %cd%\UnrealProject.uproject -NetTrace=1 -trace=net
```

---

## Actor Replication Frequency

* 서버, 클라이언트 통신 빈도를 의미
* NetUpdateFrequency - Frequency 최대치 설정
    * 단, 최대치이지 보장한다는 의미는 아님
    * 서버의 Tick에 달림

```cpp
// MyActor.cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 이 Actor의 업데이트 빈도를 초당 20회로 설정
    NetUpdateFrequency = 20.0f;
}
```

---

## 적응형 네트워크 업데이트 (Adaptive Network Update)

* 유의미한 업데이트가 없다면 빈도수를 줄여준다
    * `MinNetUpdateFrequency`: 빈도 최소치 지정

```ini
[SystemSettings]
net.UseAdaptiveNetUpdateFrequncy=1
```

---

## Replication 연관성 (Relevancy)

* 서버의 관점에서 접속한 클라이언트의 액터가 자신과 관련된(랜더링해야할) 액터인지 판별해 액터정보를 송신
* 서버와 클라이언트 간의 데이터 **통신을 최적화**하기 위해 사용됩니다. 
* 서버가 각 클라이언트에게 어떤 **데이터를 전송할지 결정**하는 데 도움을 줍니다. 
    * 데이터 필터링: 모든 객체를 모든 클라이언트에게 전송하는 것은 비효율적입니다. 대신, Replication Relevancy는 클라이언트에게 중요하거나 "관련성이 있는" 객체만 전송합니다.
    * 클라이언트 관점: 서버는 각 클라이언트의 위치, 시야, 그리고 다른 요소들을 고려하여, 해당 클라이언트에게 중요한 객체만을 결정합니다.
    * 퍼포먼스 최적화: 이 시스템을 통해 네트워크 대역폭과 처리 시간을 줄일 수 있어, 더 많은 플레이어와 복잡한 시나리오를 지원할 수 있습니다.

```cpp
#include "GameFramework/Actor.h"

// 가정: MyGameActor라는 Actor 클래스가 있고, 이를 상속받아 사용합니다.
class AMyGameActor : public AActor
{
    // ... 클래스 멤버와 기타 필요한 함수들 ...

    // IsNetRelevantFor 함수 오버라이드
    virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override
    {
        // RealViewer: 현재 데이터를 받을 클라이언트를 대표하는 플레이어 객체
        // ViewTarget: 클라이언트가 현재 제어하고 있는 객체(예: 캐릭터)
        // SrcLocation: 이 객체의 위치

        // 간단한 거리 기반 체크
        // 여기서는 객체가 플레이어로부터 일정 거리 이내에 있을 때만 관련성이 있다고 판단합니다.
        const float MaxRelevantDistance = 1000.0f; // 일정 거리 (예: 1000 유닛)
        float Dist = FVector::Dist(ViewTarget->GetActorLocation(), SrcLocation);

        if (Dist <= MaxRelevantDistance)
        {
            // 플레이어와 충분히 가까우면, 이 객체는 관련성이 있다고 판단합니다.
            return true;
        }

        // 그렇지 않으면, 이 객체는 해당 클라이언트에게 전송되지 않습니다.
        return false;
    }
};

```

### Actor 연관성에 관한 속성

* **AlwaysRelevant**
    * 객체가 모든 클라이언트에게 항상 관련성이 있다고 설정하는 변수입니다. 
    * 이 설정은 주로 게임의 핵심적인 요소나 항상 갱신되어야 하는 중요한 정보를 가진 객체에 사용됩니다.

```cpp
// AMyGameActor.h
class AMyGameActor : public AActor
{
    // ... 클래스 멤버와 기타 필요한 함수들 ...
};
// AMyGameActor.cpp
AMyGameActor::AMyGameActor()
{
    // 이 객체를 항상 관련성 있게 설정
    bAlwaysRelevant = true;
}
```

* **NetUseOwnerRelevancy**
    * 설정은 객체의 관련성이 해당 객체의 "소유자(owner)"에 의존하도록 합니다. 
    * 즉, 소유자가 관련성이 있는 클라이언트에게만 이 객체도 전송됩니다.

```cpp
// AMyGameActor.cpp
AMyGameActor::AMyGameActor()
{
    // 소유자의 관련성을 사용하도록 설정
    bNetUseOwnerRelevancy = true;
}

```

* **OnlyRelevancyToOwner**
    *  이 설정은 객체가 오직 그것의 소유자에게만 관련성이 있다고 설정합니다. 
    * 이는 플레이어의 개인적인 아이템이나 UI 요소 같은 것들에 유용합니다.

```cpp
// AMyGameActor.cpp
AMyGameActor::AMyGameActor()
{
    // 오직 소유자에게만 관련성 있게 설정
    bOnlyRelevantToOwner = true;
}

```

* **NetCullDistanceSquared**
    * 객체가 클라이언트로부터 얼마나 멀리 떨어져 있을 때 관련성을 잃을지 결정하는 거리 값입니다. 
    * 거리는 제곱 형태로 저장되므로, 계산 시 제곱근을 취하지 않아도 됩니다. 이는 성능 최적화를 위한 것입니다.

```cpp
// AMyGameActor.cpp
AMyGameActor::AMyGameActor()
{
    // 관련성 산정 거리 설정 (예: 5000 유닛)
    NetCullDistanceSquared = 5000.0f * 5000.0f;
}

```