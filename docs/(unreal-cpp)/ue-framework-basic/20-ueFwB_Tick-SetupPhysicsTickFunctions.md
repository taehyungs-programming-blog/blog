---
layout: default
title: "20. UWorld::SetupPhysicsTickFunctions"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 개요
이 함수는 Unreal Engine의 `UWorld` 클래스에서 물리 시뮬레이션을 위한 Tick Function들을 설정하는 핵심 함수입니다.

## 함수 시그니처
```cpp
void UWorld::SetupPhysicsTickFunctions(float DeltaSeconds)
```

## 주요 구성 요소

### 1. PhysicsTickFunction 기본 설정
```cpp
// StartPhysicsTickFunction 설정
StartPhysicsTickFunction.bCanEverTick = true;
StartPhysicsTickFunction.Target = this;

// EndPhysicsTickFunction 설정
EndPhysicsTickFunction.bCanEverTick = true;
EndPhysicsTickFunction.Target = this;
```

### 2. 물리 시뮬레이션 활성화 상태 확인
```cpp
bool bEnablePhysics = bShouldSimulatePhysics;
```

### 3. Tick Function 등록 상태 검증
```cpp
bool bNeedToUpdateTickRegistration = 
    (bEnablePhysics != StartPhysicsTickFunction.IsTickFunctionRegistered()) || 
    (bEnablePhysics != EndPhysicsTickFunction.IsTickFunctionRegistered());
```

## Tick Function 등록/해제 로직

### StartPhysicsTickFunction 처리
```cpp
if (bEnablePhysics && !StartPhysicsTickFunction.IsTickFunctionRegistered())
{
    StartPhysicsTickFunction.TickGroup = TG_StartPhysics;
    StartPhysicsTickFunction.RegisterTickFunction(PersistentLevel);
}
else if (!bEnablePhysics && StartPhysicsTickFunction.IsTickFunctionRegistered())
{
    StartPhysicsTickFunction.UnRegisterTickFunction();
}
```

### EndPhysicsTickFunction 처리
```cpp
if (bEnablePhysics && !EndPhysicsTickFunction.IsTickFunctionRegistered())
{
    EndPhysicsTickFunction.TickGroup = TG_EndPhysics;
    EndPhysicsTickFunction.RegisterTickFunction(PersistentLevel);
    
    // StartPhysicsTickFunction을 선행 조건으로 설정
    EndPhysicsTickFunction.AddPrerequisite(this, StartPhysicsTickFunction);
}
else if (!bEnablePhysics && EndPhysicsTickFunction.IsTickFunctionRegistered())
{
    EndPhysicsTickFunction.RemovePrerequisite(this, StartPhysicsTickFunction);
    EndPhysicsTickFunction.UnRegisterTickFunction();
}
```

## 핵심 동작 원리

### 1. Tick Group 할당
- **StartPhysicsTickFunction**: `TG_StartPhysics` 그룹에 할당
- **EndPhysicsTickFunction**: `TG_EndPhysics` 그룹에 할당

### 2. 실행 순서 보장
- `EndPhysicsTickFunction`이 `StartPhysicsTickFunction`의 완료를 기다리도록 선행 조건(Prerequisite) 설정
- 이를 통해 물리 시뮬레이션의 시작과 끝이 올바른 순서로 실행됨

### 3. 동적 등록/해제
- 물리 시뮬레이션 활성화 상태에 따라 Tick Function들을 동적으로 등록하거나 해제
- 불필요한 성능 오버헤드 방지

## 조건부 실행
- `PersistentLevel`이 존재할 때만 Tick Function 등록/해제 수행
- 안전한 레벨 상태에서만 물리 시스템 초기화

## 성능 최적화
- `bNeedToUpdateTickRegistration` 플래그를 통해 불필요한 등록/해제 작업 방지
- 현재 상태와 목표 상태를 비교하여 필요한 경우에만 업데이트 수행
