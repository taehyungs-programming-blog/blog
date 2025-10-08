---
layout: default
title: "19. Unreal Engine Tick 처리 전체 과정"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 실제 UWorld::Tick 코드 구조

```cpp
void UWorld::Tick( ELevelTick TickType, float DeltaSeconds )
{
    // 1. World Tick 시작 이벤트 호출
    FWorldDelegates::OnWorldTickStart.Broadcast(this, TickType, DeltaSeconds);

    AWorldSettings* Info = GetWorldSettings();
    NavigationSystem->Tick(DeltaSeconds);

    // 2. Actor Tick 전 이벤트 호출
    FWorldDelegates::OnWorldPreActorTick.Broadcast(this, TickType, DeltaSeconds);

    // 3. Tick이 허용된 Level만 수집
    TArray<ULevel*> LevelsToTick;
    for (ULevel* CollectionLevel : LevelCollections[i].GetLevels())
    {
        const bool bAddToTickList = Levels.Contains(CollectionLevel);
        if (bAddToTickList && CollectionLevel)
        {
            LevelsToTick.Add(CollectionLevel);
        }
    }

    // 4. Tick 수행 조건 확인
    bool bDoingActorTicks = (TickType!=LEVELTICK_TimeOnly) && !bIsPaused;
    if (bDoingActorTicks)
    {
        // 5. PhysicsTickFunction 설정 (등록 또는 해제)
        SetupPhysicsTickFunctions(DeltaSeconds);

        // 6. TickGroup 초기화
        TickGroup = TG_PrePhysics;

        // 7. 모든 Level의 TickFunction을 Queue에 저장
        FTickTaskManager::Get().StartFrame(this, DeltaSeconds, TickType, LevelsToTick);

        // 8. TickGroup별로 순차 실행
        RunTickGroup(TG_PrePhysics);
        RunTickGroup(TG_StartPhysics);
        RunTickGroup(TG_DuringPhysics, false);
        RunTickGroup(TG_EndPhysics);
        RunTickGroup(TG_PostPhysics);
    }

    // 9. TimerManager Tick 처리
    GetTimerManager().Tick(DeltaSeconds);

    // 10. TickableGameObject Tick 처리
    FTickableGameObject::TickObjects(this, TickType, bIsPaused, DeltaSeconds);

    // 11. Camera View Update
    for( FConstPlayerControllerIterator Iterator = GetPlayerControllerIterator(); Iterator; ++Iterator )
    {
        if (APlayerController* PlayerController = Iterator->Get())
        {
            PlayerController->UpdateCameraManager(DeltaSeconds);
        }
    }

    // 12. 특수 TickGroup 실행
    if (bDoingActorTicks)
    {
        RunTickGroup(TG_PostUpdateWork);
        RunTickGroup(TG_LastDemotable);
    }

    // 13. TickTaskManager 프레임 종료 처리
    FTickTaskManagerInterface::Get().EndFrame();

    // 14. 가비지 컬렉션 (평균 61초마다)
    GEngine->ConditionalCollectGarbage();

    // 15. World Tick 종료 이벤트 호출
    FWorldDelegates::OnWorldTickEnd.Broadcast(this, TickType, DeltaSeconds);
}
```

## Tick 처리 단계별 상세 설명

### 1. World Tick 시작 이벤트
```cpp
FWorldDelegates::OnWorldTickStart.Broadcast(this, TickType, DeltaSeconds);
```
- World Tick이 시작될 때 호출되는 델리게이트
- 외부 시스템들이 Tick 시작을 감지할 수 있도록 함

### 2. Navigation System Tick
```cpp
NavigationSystem->Tick(DeltaSeconds);
```
- 네비게이션 시스템의 독립적인 Tick 처리
- AI 경로 찾기와 관련된 업데이트

### 3. Tick 가능한 Level 수집
```cpp
TArray<ULevel*> LevelsToTick;
for (ULevel* CollectionLevel : LevelCollections[i].GetLevels())
{
    const bool bAddToTickList = Levels.Contains(CollectionLevel);
    if (bAddToTickList && CollectionLevel)
    {
        LevelsToTick.Add(CollectionLevel);
    }
}
```
- 현재 활성화된 Level들만 수집
- 불필요한 Level의 Tick을 방지하여 성능 최적화

### 4. Tick 수행 조건 확인
```cpp
bool bDoingActorTicks = (TickType!=LEVELTICK_TimeOnly) && !bIsPaused;
```
- `LEVELTICK_TimeOnly`: 시간만 갱신하는 경우 (Actor Tick 제외)
- `bIsPaused`: 게임이 일시정지 상태인지 확인
- 두 조건을 모두 만족해야 Actor Tick 실행

### 5. PhysicsTickFunction 설정
```cpp
SetupPhysicsTickFunctions(DeltaSeconds);
```
- 물리 시뮬레이션 관련 TickFunction들을 설정
- StartPhysicsTickFunction, EndPhysicsTickFunction 등록/해제
- 물리 시뮬레이션의 시작과 끝을 관리

### 6. TickGroup 초기화
```cpp
TickGroup = TG_PrePhysics;
```
- 현재 TickGroup을 `TG_PrePhysics`로 초기화
- TickGroup은 실행 순서를 결정하는 핵심 요소

### 7. TickTaskManager StartFrame
```cpp
FTickTaskManager::Get().StartFrame(this, DeltaSeconds, TickType, LevelsToTick);
```
- 모든 Level의 TickFunction들을 분석하고 Queue에 저장
- TickFunction의 실행 조건과 의존성을 확인
- CoolingDown 상태를 체크하여 실행 가능한 TickFunction만 선별

### 8. TickGroup별 순차 실행
```cpp
RunTickGroup(TG_PrePhysics);      // 물리 전 처리
RunTickGroup(TG_StartPhysics);    // 물리 시작
RunTickGroup(TG_DuringPhysics, false); // 물리 중 (병렬)
RunTickGroup(TG_EndPhysics);      // 물리 종료
RunTickGroup(TG_PostPhysics);     // 물리 후 처리
```
- 각 TickGroup은 특정 시점의 TickFunction들을 그룹화
- 물리 시뮬레이션을 중심으로 실행 순서가 결정됨
- `TG_DuringPhysics`는 병렬 실행 가능

### 9. TimerManager Tick
```cpp
GetTimerManager().Tick(DeltaSeconds);
```
- 게임 내 타이머들의 업데이트
- 지연 실행, 반복 실행 등의 타이머 로직 처리

### 10. TickableGameObject Tick
```cpp
FTickableGameObject::TickObjects(this, TickType, bIsPaused, DeltaSeconds);
```
- TickableGameObject 인터페이스를 구현한 객체들의 Tick
- World와 독립적으로 Tick이 필요한 객체들 처리

### 11. Camera View Update
```cpp
for( FConstPlayerControllerIterator Iterator = GetPlayerControllerIterator(); Iterator; ++Iterator )
{
    if (APlayerController* PlayerController = Iterator->Get())
    {
        PlayerController->UpdateCameraManager(DeltaSeconds);
    }
}
```
- 모든 플레이어 컨트롤러의 카메라 매니저 업데이트
- 카메라 위치, 회전 등의 최종 업데이트

### 12. 특수 TickGroup 실행
```cpp
RunTickGroup(TG_PostUpdateWork);
RunTickGroup(TG_LastDemotable);
```
- 모든 주요 Tick이 완료된 후의 후처리 작업
- UI 업데이트, 최종 정리 작업 등

### 13. TickTaskManager EndFrame
```cpp
FTickTaskManagerInterface::Get().EndFrame();
```
- 현재 프레임의 Tick 처리 완료
- TickFunction들의 상태 업데이트
- 다음 프레임을 위한 준비 작업

### 14. 가비지 컬렉션
```cpp
GEngine->ConditionalCollectGarbage();
```
- 평균 61초마다 실행되는 가비지 컬렉션
- 메모리 정리 및 최적화

### 15. World Tick 종료 이벤트
```cpp
FWorldDelegates::OnWorldTickEnd.Broadcast(this, TickType, DeltaSeconds);
```
- World Tick이 완료될 때 호출되는 델리게이트
- 외부 시스템들이 Tick 완료를 감지할 수 있도록 함

## TickGroup 실행 순서

1. **TG_PrePhysics**: 물리 시뮬레이션 전 처리
2. **TG_StartPhysics**: 물리 시뮬레이션 시작
3. **TG_DuringPhysics**: 물리 시뮬레이션 중 (병렬 실행 가능)
4. **TG_EndPhysics**: 물리 시뮬레이션 종료
5. **TG_PostPhysics**: 물리 시뮬레이션 후 처리
6. **TG_PostUpdateWork**: 후처리 작업
7. **TG_LastDemotable**: 최종 정리 작업

## 핵심 특징

### 1. 조건부 실행
- `LEVELTICK_TimeOnly`와 `bIsPaused` 상태에 따라 Actor Tick 생략 가능
- 성능 최적화를 위한 스마트한 실행 제어

### 2. 그룹화된 실행
- TickGroup을 통한 체계적인 실행 순서 관리
- 물리 시뮬레이션을 중심으로 한 논리적 그룹화

### 3. 병렬 처리 지원
- `TG_DuringPhysics`는 병렬 실행 가능
- 멀티스레딩을 통한 성능 향상

### 4. 이벤트 기반 확장성
- 델리게이트를 통한 외부 시스템 연동
- 모듈화된 구조로 확장성 제공

## 성능 최적화 요소

1. **Level 기반 필터링**: 활성화된 Level만 Tick 처리
2. **조건부 실행**: 불필요한 Tick 생략
3. **병렬 처리**: 물리 시뮬레이션 중 병렬 실행
4. **그룹화**: 관련된 TickFunction들을 그룹으로 묶어 효율적 처리
5. **가비지 컬렉션**: 주기적인 메모리 정리

> 이제 좀 더 상세히 보자.