---
layout: default
title: "02. GEngineLoop에서 Tick까지"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 정의

* **GEngineLoop**는 Unreal Engine의 전체 실행 흐름을 관리하는 메인 루프 컨트롤러
    * 쉽게 말해, 엔진이 실행될 때 초기화 → 게임 루프 실행 → 종료 처리까지 모든 단계를 총괄하는 "심장 박동기" 같은 역할

```cpp
int32 GuardedMain(const TCHAR* CmdLine)
{
    GEngineLoop.PreInit(CmdLine);  // 초기화

    while (!GIsRequestingExit)     // 게임 루프
    {
        GEngineLoop.Tick();
    }

    GEngineLoop.Exit();            // 종료
    return 0;
}
```

---

## GEngineLoop에서 Tick까지

```cpp
// 의사코드 (개념 흐름)
int32 GuardedMain(const TCHAR* CmdLine)
{
    GEngineLoop.PreInit(CmdLine);

    while (!GIsRequestingExit)
    {
        GEngineLoop.Tick(); // ← 여기서 한 프레임의 모든 일이 시작
    }

    GEngineLoop.Exit();
    return 0;
}
```

```cpp
void FEngineLoop::Tick()
{
    // 1) 플랫폼 메시지 펌프(윈도우 메시지, 입력 등)
    // 2) 시간 계산(DeltaSeconds)
    // 3) 엔진 틱
    EngineTick(DeltaSeconds, /*bIdleMode=*/false);
}
```

```cpp
void EngineTick(float DeltaSeconds, bool bIdle)
{
    if (GEngine) // UGameEngine*
    {
        GEngine->Tick(DeltaSeconds, bIdle); // ← 핵심 위임
    }
}
```

```cpp
void UGameEngine::Tick(float DeltaSeconds, bool bIdle)
{
    for (UWorld* World : GetWorldContexts())
    {
        if (World) { World->Tick(ELevelTick::LEVELTICK_All, DeltaSeconds); }
    }
}
```

* UWorld::Tick() 내부에서 일어나는 핵심 포인트:
    * 시간/물리/네비게이션 등 프레임 준비
* Tick 함수 수집 & 스케줄링: 월드에 존재하는 모든 TickFunction(액터/컴포넌트)을 FTickTaskManager에 등록
* TickGroup 순서대로 실행:
    * TG_PrePhysics → TG_StartPhysics → TG_DuringPhysics → TG_EndPhysics → TG_PostPhysics(대표 흐름)
    * 각 TickFunction이 실행되며 개별 Actor/Component의 Tick이 호출

```cpp
void UWorld::Tick(ELevelTick LevelTick, float DeltaSeconds)
{
    // 프레임 준비...
    FTickTaskManager& TTM = GetTickTaskManager();

    // 1) 이 프레임에 틱해야 할 TickFunction들(액터/컴포넌트)을 수집
    TTM.StartFrame(this, DeltaSeconds);

    // 2) TickGroup별 실행 (의존성/선행조건 보장)
    for (ETickingGroup Group : TickGroupsInOrder)
    {
        TTM.RunTickGroup(Group, DeltaSeconds);
    }

    // 3) 후처리(가비지 콜렉션, 렌더 커맨드 플러시 등)
}
```

### 액터가 틱을 받으려면

* `PrimaryActorTick.bCanEverTick = true;`
* 비활성화되지 않았을 것(SetActorTickEnabled(true))
* 월드에서 가시성/활성화/네트워크 조건을 충족
* 틱 간격(TickInterval)을 만족(스킵되지 않음)
* 지정한 TickGroup 타이밍이 도래

```cpp
// 예시 액터
class AMyActor : public AActor
{
    GENERATED_BODY()
public:
    AMyActor()
    {
        PrimaryActorTick.bCanEverTick = true;  // 틱 가능 선언
        PrimaryActorTick.TickGroup = TG_PrePhysics; // 원하는 그룹
        PrimaryActorTick.TickInterval = 0.f;   // 매 프레임
    }

    virtual void Tick(float DeltaSeconds) override
    {
        // 매 프레임 로직
    }
};
```

```
GEngineLoop::Tick
  → EngineTick
    → UGameEngine::Tick
      → UWorld::Tick
        → FTickTaskManager(그룹별 실행)
          → FActorTickFunction::ExecuteTick
            → AActor::TickActor()
              → AActor::Tick(DeltaSeconds)   // 당신이 오버라이드한 함수
              → (블루프린트라면 ReceiveTick 이벤트 디스패치)
          → UActorComponent::PrimaryComponentTick.ExecuteTick
            → UActorComponent::TickComponent(DeltaSeconds)

```