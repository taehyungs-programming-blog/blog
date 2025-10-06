---
layout: default
title: "15. Tick 이론론"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
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