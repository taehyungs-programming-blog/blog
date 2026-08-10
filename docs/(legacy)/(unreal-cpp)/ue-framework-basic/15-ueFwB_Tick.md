---
layout: default
title: "15. Tick 이론"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

# UE Tick 시스템 이론 가이드

## 1. UE Tick 시스템 개요

UE(Unreal Engine)의 Tick 시스템은 게임의 매 프레임마다 실행되는 업데이트 메커니즘입니다. 이 시스템을 통해 게임 오브젝트들이 시간에 따라 변화하는 로직을 구현할 수 있습니다.

### 핵심 개념
- **Tick Function**: 매 프레임마다 호출되는 함수
- **TickTaskLevel**: Tick 함수들을 관리하는 시스템
- **Register/Unregister**: Tick 함수를 시스템에 등록/해제하는 과정

## 2. Tick Function 상속과 Register 과정

### 2.1 기본 구조
```cpp
// 기본 Tick Function 구조
class UMyComponent : public UActorComponent
{
public:
    UMyComponent();
    
    // Tick Function 선언
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                              FActorComponentTickFunction* ThisTickFunction) override;
    
protected:
    // Tick 활성화/비활성화
    virtual void BeginPlay() override;
};
```

### 2.2 Register 과정
```cpp
void UMyComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Tick 활성화 (Register는 자동으로 처리됨)
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}
```

### 2.3 모든 Component에서 Tick 상속 후 Register하면 Tick을 받나?
**정답: 네!** 모든 Component에서 Tick을 상속하고 Register하면 Tick을 받을 수 있습니다.

```cpp
// 예시: 모든 Component가 Tick을 받는 경우
class UHealthComponent : public UActorComponent
{
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                              FActorComponentTickFunction* ThisTickFunction) override
    {
        // 매 프레임 체력 회복 로직
        CurrentHealth += HealthRegenRate * DeltaTime;
    }
};

class UMovementComponent : public UActorComponent
{
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                              FActorComponentTickFunction* ThisTickFunction) override
    {
        // 매 프레임 이동 로직
        UpdateMovement(DeltaTime);
    }
};
```

## 3. UObject vs AActor Tick 구현 차이점

### 3.1 UObject에서 Tick 구현의 문제점

**질문: UObject에서 Tick을 구현해 둘까?**
**답변: 아니요!** 안 쓸 경우가 더 많을 것이고, 이는 오버헤드가 큽니다.

```cpp
// 나쁜 예시: UObject에 Tick 구현
void UObject::Tick(float DeltaSeconds)
{
    if (bTick) // 매 프레임 호출되는 Tick에겐 엄청난 오버헤드
    {
        // 실제 로직
    }
}
```

**문제점:**
- 매 프레임마다 `if (bTick)` 체크가 발생
- 대부분의 UObject는 정적이므로 불필요한 오버헤드
- 메모리 사용량 증가

### 3.2 AActor에서 Tick 구현
```cpp
// AActor의 Tick 구현
class AMyActor : public AActor
{
public:
    AMyActor();
    
    virtual void Tick(float DeltaTime) override;
    
protected:
    virtual void BeginPlay() override;
};
```

### 3.3 struct에서 Tick을 받고 싶다면?
**UE의 해결책: Tick Function을 직접 만들자!**

```cpp
// Custom Tick Function 구현
class UMyComponent : public UActorComponent
{
public:
    UMyComponent();
    
    // Custom Tick Function
    FTickFunction MyCustomTickFunction;
    
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    // Custom Tick 함수
    void MyCustomTick(float DeltaTime);
    
    // Tick Function 실행 함수
    static void ExecuteTick(FTickFunction& TickFunction, float DeltaTime);
};
```

## 4. Tick Function 직접 구현 방법

### 4.1 Custom Tick Function 설정
```cpp
UMyComponent::UMyComponent()
{
    // Custom Tick Function 초기화
    MyCustomTickFunction.TickGroup = TG_PrePhysics;
    MyCustomTickFunction.EndTickGroup = TG_PostPhysics;
    MyCustomTickFunction.bCanEverTick = true;
    MyCustomTickFunction.bStartWithTickEnabled = true;
    MyCustomTickFunction.bAllowTickOnDedicatedServer = true;
    MyCustomTickFunction.bHighPriority = false;
    MyCustomTickFunction.TickInterval = 0.0f;
}

void UMyComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Custom Tick Function 등록
    MyCustomTickFunction.RegisterTickFunction(GetWorld());
}

void UMyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Custom Tick Function 해제
    MyCustomTickFunction.UnRegisterTickFunction();
    
    Super::EndPlay(EndPlayReason);
}
```

### 4.2 Tick Function 실행 함수 구현
```cpp
void UMyComponent::ExecuteTick(FTickFunction& TickFunction, float DeltaTime)
{
    // TickFunction의 Target을 통해 Component에 접근
    UMyComponent* Component = Cast<UMyComponent>(TickFunction.Target);
    if (Component)
    {
        Component->MyCustomTick(DeltaTime);
    }
}

void UMyComponent::MyCustomTick(float DeltaTime)
{
    // 실제 Tick 로직 구현
    UE_LOG(LogTemp, Warning, TEXT("Custom Tick executed: %f"), DeltaTime);
}
```

## 5. 성능 최적화 관점에서의 Tick 설계 원칙

### 5.1 Tick 최적화 전략
```cpp
class UOptimizedComponent : public UActorComponent
{
public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                              FActorComponentTickFunction* ThisTickFunction) override
    {
        // 1. 필요한 경우에만 Tick 실행
        if (!ShouldTick())
        {
            return;
        }
        
        // 2. Tick 간격 조절
        static float TickInterval = 0.1f; // 100ms마다 실행
        static float LastTickTime = 0.0f;
        
        if (GetWorld()->GetTimeSeconds() - LastTickTime < TickInterval)
        {
            return;
        }
        LastTickTime = GetWorld()->GetTimeSeconds();
        
        // 3. 실제 로직 실행
        PerformOptimizedLogic(DeltaTime);
    }
    
private:
    bool ShouldTick() const
    {
        // Tick이 필요한 조건 체크
        return bIsActive && bNeedsUpdate;
    }
    
    void PerformOptimizedLogic(float DeltaTime)
    {
        // 최적화된 로직
    }
};
```

### 5.2 Tick 그룹 활용
```cpp
// Tick 그룹별 최적화
class UPhysicsComponent : public UActorComponent
{
public:
    UPhysicsComponent()
    {
        // 물리 계산 전에 실행
        PrimaryComponentTick.TickGroup = TG_PrePhysics;
    }
};

class URenderComponent : public UActorComponent
{
public:
    URenderComponent()
    {
        // 렌더링 전에 실행
        PrimaryComponentTick.TickGroup = TG_PostPhysics;
    }
};
```

## 6. 실제 사용 예시

### 6.1 게임플레이 컴포넌트 예시
```cpp
class UGameplayComponent : public UActorComponent
{
public:
    UGameplayComponent();
    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                              FActorComponentTickFunction* ThisTickFunction) override;
    
    // 게임플레이 로직
    void UpdateGameplay(float DeltaTime);
    void CheckWinCondition();
    void UpdateScore();
    
private:
    float GameplayTimer;
    int32 CurrentScore;
    bool bGameActive;
};
```

### 6.2 AI 컴포넌트 예시
```cpp
class UAIComponent : public UActorComponent
{
public:
    UAIComponent();
    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                              FActorComponentTickFunction* ThisTickFunction) override;
    
    // AI 로직
    void UpdateAI(float DeltaTime);
    void ProcessAIState();
    void MakeDecision();
    
private:
    float AITimer;
    EAIState CurrentState;
    float DecisionInterval;
};
```

## 7. 결론

UE Tick 시스템은 다음과 같은 원칙으로 설계되어 있습니다:

1. **필요한 경우에만 Tick 사용**: 모든 오브젝트에 Tick을 구현하지 말고, 필요한 경우에만 사용
2. **성능 최적화**: Tick 간격 조절, 조건부 실행 등을 통해 성능 최적화
3. **적절한 Tick 그룹 사용**: 물리, 렌더링 등 적절한 시점에 Tick 실행
4. **Custom Tick Function 활용**: 특별한 요구사항이 있을 때 직접 구현

이러한 원칙들을 따라 Tick 시스템을 구현하면 효율적이고 성능이 좋은 게임을 만들 수 있습니다.

---

## Tips) bCanEverTick Vs bStartWithTickEnabled

### bCanEverTick 사용 시기:
- 오브젝트가 **언제든지** Tick을 받을 수 있어야 할 때
- 런타임에 Tick 활성화/비활성화가 필요할 때
- 성능 최적화를 위해 조건부 Tick이 필요할 때

### bStartWithTickEnabled 사용 시기:
- 오브젝트 생성 시점의 Tick 상태를 제어할 때
- 초기에는 비활성화하고 나중에 활성화할 때
- 이벤트 기반으로 Tick을 제어할 때

### 성능 최적화 팁:
1. **bCanEverTick = false**: Tick이 전혀 필요하지 않은 경우
2. **bCanEverTick = true, bStartWithTickEnabled = false**: 조건부 Tick이 필요한 경우
3. **bCanEverTick = true, bStartWithTickEnabled = true**: 항상 Tick이 필요한 경우

---

### 1. FTickTaskLevel

`FTickTaskLevel`은 특정 레벨(ULevel)에 대한 틱 작업을 관리하는 구조체

#### 주요 특징:
- **레벨별 독립성**: 각 레벨은 자체적인 틱 작업 목록을 가짐
- **효율적인 메모리 관리**: 레벨 로딩/언로딩 시 틱 작업의 할당과 해제를 최적화
- **격리된 실행**: 레벨 간 틱 작업의 독립적인 처리 보장

#### 구조:
```cpp
struct FTickTaskLevel
{
    // 레벨 내의 모든 틱 함수들을 추적
    TArray<FTickFunction*> TickFunctions;
    
    // 레벨별 틱 작업 관리
    void RegisterTickFunction(FTickFunction* TickFunction);
    void UnregisterTickFunction(FTickFunction* TickFunction);
};
```

### 2. FTickTaskSequencer

`FTickTaskSequencer`는 틱 작업의 실행 순서를 조율하는 핵심 컴포넌트

#### 주요 기능:
- **순서 조율**: 각 틱 그룹(Tick Group)에 대한 작업 관리
- **우선순위 처리**: 종속성이 있는 작업들의 올바른 실행 순서 보장
- **병렬 처리**: 독립적인 작업들의 동시 실행 지원

#### Tick Group 예시:
- **TG_PrePhysics**: 물리 연산 전 실행
- **TG_DuringPhysics**: 물리 연산 중 실행
- **TG_PostPhysics**: 물리 연산 후 실행
- **TG_PostUpdateWork**: 모든 업데이트 완료 후 실행

#### 구조:
```cpp
class FTickTaskSequencer
{
    // 각 틱 그룹별 작업 관리
    TMap<ETickingGroup, TArray<FTickFunction*>> TickGroups;
    
    // 작업 순서 조율
    void SortTickGroups();
    void ExecuteTickGroup(ETickingGroup Group);
};
```

### 3. FTickTaskManager

`FTickTaskManager`는 전체 틱 시스템을 총괄하는 매니저 클래스

#### 주요 책임:
- **전역 관리**: 모든 레벨의 `FTickTaskLevel` 관리
- **프레임별 처리**: 매 프레임마다 틱 작업의 수집과 실행
- **생명주기 관리**: 틱 작업의 등록, 해제, 실행 순서 조율

#### 핵심 메서드:

##### StartFrame()
```cpp
void FTickTaskManager::StartFrame()
{
    // 프레임 시작 시 모든 틱 작업 준비
    // 레벨별 틱 작업 수집
    // 실행 순서 최적화
}
```

##### RunTickGroup()
```cpp
void FTickTaskManager::RunTickGroup(ETickingGroup Group)
{
    // 특정 틱 그룹의 모든 작업 실행
    // 병렬 처리 가능한 작업들 동시 실행
    // 종속성 있는 작업들 순차 실행
}
```

##### EndFrame()
```cpp
void FTickTaskManager::EndFrame()
{
    // 프레임 종료 시 정리 작업
    // 다음 프레임을 위한 준비
    // 메모리 정리 및 상태 초기화
}
```

## 시스템 아키텍처

### 계층 구조
```
FTickTaskManager (전역 관리자)
    ├── FTickTaskLevel (레벨별 관리)
    │   ├── TickFunction 1
    │   ├── TickFunction 2
    │   └── ...
    └── FTickTaskSequencer (순서 조율)
        ├── TG_PrePhysics
        ├── TG_DuringPhysics
        ├── TG_PostPhysics
        └── TG_PostUpdateWork
```

### 실행 흐름
1. **프레임 시작**: `StartFrame()` 호출
2. **틱 작업 수집**: 모든 레벨에서 틱 함수 수집
3. **순서 조율**: `FTickTaskSequencer`가 실행 순서 결정
4. **그룹별 실행**: 각 틱 그룹을 순서대로 실행
5. **프레임 종료**: `EndFrame()` 호출

## 성능 최적화

### 병렬 처리
- 독립적인 틱 작업들의 동시 실행
- 멀티스레딩을 통한 성능 향상
- CPU 코어 활용도 극대화

### 메모리 효율성
- 레벨별 틱 작업 격리
- 동적 할당 최소화
- 캐시 친화적인 데이터 구조

### 실행 순서 최적화
- 종속성 분석을 통한 최적 실행 순서 결정
- 불필요한 대기 시간 제거
- 프레임 시간 안정성 보장

## 사용 예시

### 틱 함수 등록
```cpp
// 액터의 틱 함수 등록
void AMyActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 틱 함수 등록
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;
    PrimaryActorTick.TickInterval = 0.0f; // 매 프레임 실행
}
```

### 커스텀 틱 그룹 사용
```cpp
// 커스텀 틱 함수 생성
FTickFunction MyTickFunction;
MyTickFunction.TickGroup = TG_PostPhysics;
MyTickFunction.bCanEverTick = true;
MyTickFunction.bStartWithTickEnabled = true;
MyTickFunction.SetTickFunctionEnable(true);
```

## 결론

Unreal Engine의 Tick 시스템은 `FTickTaskLevel`, `FTickTaskSequencer`, `FTickTaskManager`의 세 가지 핵심 구성 요소로 이루어진 정교한 아키텍처를 가지고 있습니다. 이 시스템은:

- **확장성**: 새로운 틱 작업의 쉬운 추가
- **성능**: 병렬 처리와 최적화된 실행 순서
- **안정성**: 레벨별 격리와 종속성 관리
- **유연성**: 다양한 틱 그룹과 우선순위 지원

