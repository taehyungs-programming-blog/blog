---
layout: default
title: "17. Tick Group"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

# Unreal Engine Tick Group 가이드

## 개요

* Tick Group은 Unreal Engine에서 컴포넌트와 액터의 업데이트 순서를 제어하는 시스템
* 이를 통해 물리 시뮬레이션과 게임 로직이 올바른 순서로 실행되도록 보장

## Tick Group이 필요한 이유

### 문제 상황
카메라 촬영 후 Component Tick을 활성화한다고 생각해보면, 항상 한 틱 밀려서 그려지게 됩니다. 이는 다음과 같은 문제를 야기

- **타이밍 문제**: 물리 시뮬레이션과 렌더링이 동기화되지 않음
- **예측 불가능성**: 업데이트 순서가 일관되지 않음
- **성능 저하**: 불필요한 재계산과 지연 발생

### 해결책
Tick Group을 통해 각 컴포넌트가 언제 실행되어야 하는지 명확히 정의하여, 올바른 순서로 업데이트가 이루어지게한다

## Tick Group 종류

### 1. TG_PrePhysics
- **설명**: 물리 시뮬레이션이 시작되기 전에 실행해야 하는 모든 항목
- **사용 예시**: 
  - 입력 처리
  - AI 의사결정
  - 게임 로직 업데이트
- **특징**: 물리 계산에 영향을 주는 로직이 실행됨

### 2. TG_StartPhysics (특수 그룹)
- **설명**: 물리 시뮬레이션을 시작하는 특수 틱 그룹
- **특징**: 
  - Hidden 그룹 (직접 설정 불가)
  - 물리 시뮬레이션은 예측성이 아니라 엄격한 방식으로 계산
  - 엔진 내부에서만 사용

### 3. TG_DuringPhysics
- **설명**: 물리 시뮬레이션 작업과 병행하여 실행할 수 있는 모든 항목
- **사용 예시**:
  - 독립적인 애니메이션
  - UI 업데이트
  - 네트워크 동기화
- **특징**: 물리 계산과 병렬로 실행 가능

### 4. TG_EndPhysics (특수 그룹)
- **설명**: 물리 시뮬레이션을 종료하는 특수 틱 그룹
- **특징**:
  - Hidden 그룹 (직접 설정 불가)
  - 물리 시뮬레이션 완료 후 실행
  - 엔진 내부에서만 사용

### 5. TG_PostPhysics
- **설명**: 실행하기 전에 리지드 바디 및 클로스 시뮬레이션이 완료되어야 하는 모든 아이템
- **사용 예시**:
  - 카메라 위치 업데이트
  - 충돌 결과 기반 로직
  - 물리 기반 애니메이션
- **특징**: 물리 계산 결과를 사용하는 로직

### 6. TG_PostUpdateWork
- **설명**: 체크 표시되기 전에 업데이트 작업이 필요한 모든 항목
- **사용 예시**:
  - 렌더링 준비
  - 최종 위치 계산
  - 화면 출력 준비
- **특징**: 프레임의 마지막 단계

### 7. TG_LastDemotable
- **설명**: 끝까지 강등된 모든 항목에 대한 캐치용
- **특징**: Hidden 그룹, 시스템 내부 사용

### 8. TG_NewlySpawned
- **설명**: 실제로는 틱 그룹이 아닌 특수 틱 그룹
- **특징**:
  - 모든 틱 그룹이 실행될 때마다 새로 생성된 아이템이 더 이상 없을 때까지 반복적으로 다시 실행
  - Hidden 그룹, 시스템 내부 사용

## 사용 방법

### C++에서 설정
```cpp
// 컴포넌트의 Tick Group 설정
PrimaryComponentTick.TickGroup = TG_PrePhysics;

// 특수 Tick Function 설정 (시작과 끝 그룹 지정)
PrimaryComponentTick.TickGroup = TG_PrePhysics;
PrimaryComponentTick.EndTickGroup = TG_EndPhysics;
```

### Blueprint에서 설정
1. 컴포넌트의 **Details** 패널에서 **Tick** 섹션 찾기
2. **Tick Group** 드롭다운에서 원하는 그룹 선택
3. 필요시 **End Tick Group** 설정

## 베스트 프랙티스

### 1. 적절한 Tick Group 선택
- **물리 영향**: `TG_PrePhysics` 사용
- **물리 독립적**: `TG_DuringPhysics` 사용
- **물리 결과 필요**: `TG_PostPhysics` 사용

### 2. 성능 최적화
- 불필요한 Tick 비활성화
- 적절한 Tick Group으로 분산
- `TG_DuringPhysics` 활용으로 병렬 처리

### 3. 디버깅 팁
- Tick Group 순서 확인
- 물리 시뮬레이션 타이밍 검증
- 프레임 드롭 원인 분석

## 주의사항

1. **특수 그룹 사용 금지**: `TG_StartPhysics`, `TG_EndPhysics`는 직접 설정 불가
2. **순서 의존성**: Tick Group 간 의존성 고려 필요
3. **성능 영향**: 잘못된 Tick Group 설정은 성능 저하 야기
4. **멀티플레이어**: 네트워크 동기화 시 Tick Group 순서 중요

---

# TickTaskSequencer

## 개요
TickTaskSequencer는 Unreal Engine의 Tick 시스템에서 Tick 함수들을 효율적으로 관리하고 실행 순서를 제어하는 핵심 클래스

## 3x3 간소화 모델

### TickGroup 분류 (3단계)
```
1. PrePhysics    - 물리 계산 전
2. StartPhysics  - 물리 계산 시작
3. EndPhysics    - 물리 계산 완료
```

### TickTaskSequencer 3x3 매트릭스

> 이해의 편의상 3x3이지 실제로 3x3이 아님.

| Start\End | PrePhysics | StartPhysics | EndPhysics |
|-----------|------------|--------------|------------|
| **PrePhysics** | Pre→Pre | Pre→Start | Pre→End |
| **StartPhysics** | ❌ Invalid | Start→Start | Start→End |
| **EndPhysics** | ❌ Invalid | ❌ Invalid | End→End |

### 핵심 구성 요소 (3가지)

#### 1. TickTask 배열들
```cpp
// 높은 우선순위 작업들
HiPriTickTasks[3][3]

// 기본 우선순위 작업들  
TickTasks[3][3]
```

#### 2. 완료 이벤트 관리
```cpp
// 각 TickGroup의 완료 상태 추적
TickCompletionEvents[3]
```

#### 3. 대기 및 정리
```cpp
// 다음 작업 시작을 위한 대기 그룹
WaitForTickGroup

// 메모리 정리를 위한 태스크 모음
CleanupTasks
```

## Tick Function 분류 방식

### 1. 시간 순서 기반 분류
- **PrePhysics**: 물리 계산 이전에 실행되어야 하는 로직
- **StartPhysics**: 물리 계산과 동시에 시작되는 로직  
- **EndPhysics**: 물리 계산 완료 후 실행되는 로직

### 2. Invalid 영역이 존재하는 이유
```
StartPhysics → PrePhysics: ❌ (시간 역행 불가)
EndPhysics → PrePhysics: ❌ (시간 역행 불가)  
EndPhysics → StartPhysics: ❌ (시간 역행 불가)
```

### 3. 실행 흐름 예시
```
1. PrePhysics Tick 실행
2. StartPhysics Tick 실행 (PrePhysics 완료 후)
3. EndPhysics Tick 실행 (StartPhysics 완료 후)
4. 다음 프레임으로 이동
```

## 핵심 동작 원리

### 1. 2차원 배열 관리
- `[StartGroup][EndGroup]` 형태로 Tick 작업을 분류
- 각 셀에는 해당 조건을 만족하는 Tick 함수들이 저장됨

### 2. 우선순위 처리
- `HiPriTickTasks`: 높은 우선순위 작업 (UI, 입력 등)
- `TickTasks`: 일반 우선순위 작업 (게임 로직 등)

### 3. 동기화 메커니즘
- `TickCompletionEvents`: 각 TickGroup의 완료를 보장
- `WaitForTickGroup`: 다음 작업 시작 시점 제어

## 실제 사용 예시

```cpp
// PrePhysics에서 시작해서 EndPhysics에서 끝나는 Tick 등록
RegisterTickTask(PrePhysics, EndPhysics, MyTickFunction);

// StartPhysics에서만 실행되는 Tick 등록  
RegisterTickTask(StartPhysics, StartPhysics, MyPhysicsTick);
```

## 장점

1. **명확한 실행 순서**: 시간 순서에 따른 엄격한 실행 보장
2. **효율적인 병렬 처리**: 독립적인 TickGroup들은 병렬 실행 가능
3. **메모리 최적화**: CleanupTasks를 통한 지연 해제로 성능 향상
4. **유연한 우선순위**: HiPri와 일반 우선순위로 세밀한 제어

