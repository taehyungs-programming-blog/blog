---
layout: default
title: "18. Tick InternalData"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 개요

`FInternalData`는 Unreal Engine의 Tick 시스템에서 **등록된 틱 함수의 런타임 상태를 관리**하는 핵심 데이터 구조

## 구조체 정의

```cpp
struct FTickFunction
{	
    struct FInternalData
    {
        // 등록 상태
        bool bRegistered : 1;
        
        // 실제 실행 그룹
        TEnumAsByte<enum ETickingGroup> ActualStartTickGroup;
        TEnumAsByte<enum ETickingGroup> ActualEndTickGroup;
        
        // 쿨다운 관리
        bool bWasInterval : 1;
        float RelativeTickCooldown;
        FTickFunction* Next;
        
        // 시스템 참조
        class FTickTaskLevel* TickTaskLevel;
        FBaseGraphTask* TaskPointer;
    }
    
    TUniquePtr<FInternalData> InternalData;
}
```

## 주요 멤버 변수 상세 분석

### 1. 등록 상태 관리

#### `bool bRegistered : 1`
- **목적**: 틱 함수가 실제로 등록되어 활성화되었는지 여부
- **특징**: 비트 필드 사용으로 메모리 효율성 확보
- **사용**: 런타임에서 틱 함수의 활성 상태 확인

### 2. 실제 틱 그룹 관리

#### `ActualStartTickGroup` & `ActualEndTickGroup`
- **목적**: Prerequisite에 의해 조정된 실제 실행 그룹 관리
- **설정값 vs 실제값**:
  - **설정값**: `FTickFunction`에서 사용자가 지정한 틱 그룹
  - **실제값**: 선행 조건에 의해 실제로 실행되는 틱 그룹

#### 예시 시나리오
```
TickFunction A: TG_PrePhysics 그룹 설정
TickFunction B: TG_PrePhysics 그룹 설정, A를 Prerequisite로 설정

결과:
- A: ActualStartTickGroup = TG_PrePhysics
- B: ActualStartTickGroup = TG_PrePhysics (하지만 A 완료 후 실행)
```

### 3. 쿨다운 시스템 관리

#### `bool bWasInterval : 1`
- **목적**: 현재 틱 함수가 쿨다운 목록에 있는지 여부
- **사용**: 쿨다운 상태 추적

#### `float RelativeTickCooldown`
- **목적**: 이전 틱 함수 대비 상대적 쿨다운 시간
- **계산 방식**: `현재_Interval - 이전_Interval`

#### `FTickFunction* Next`
- **목적**: 쿨다운 연결 리스트의 다음 요소 참조

#### 쿨다운 리스트 구조
```
AllCoolingDownTickFunctions
├── Head: TickFunction0 (Interval=5.0f)
│   ├── RelativeTickCooldown: 5.0f
│   └── Next: TickFunction1
├── TickFunction1 (Interval=12.0f)
│   ├── RelativeTickCooldown: 7.0f (12.0 - 5.0)
│   └── Next: TickFunction2
└── TickFunction2 (Interval=15.0f)
    ├── RelativeTickCooldown: 3.0f (15.0 - 12.0)
    └── Next: nullptr
```

### 4. 시스템 참조

#### `class FTickTaskLevel* TickTaskLevel`
- **목적**: 틱 함수가 등록된 레벨 참조
- **사용**: 레벨별 틱 관리

#### `FBaseGraphTask* TaskPointer`
- **목적**: 실제 실행될 태스크 객체 참조
- **사용**: 태스크 그래프 시스템과의 연동

## TickGroup, TickInterval, Prerequisite 관리 위치

### 질문: 어디서 관리할까?

| 항목 | 설정 위치 | 실행 관리 위치 | 설명 |
|------|-----------|----------------|------|
| **TickGroup** | `FTickFunction` | `FInternalData` | 설정값과 실제 실행 그룹 분리 관리 |
| **TickInterval** | `FTickFunction` | `FInternalData` | 쿨다운 계산에 활용 |
| **Prerequisite** | `FTickFunction` | `FInternalData` | `ActualStartTickGroup`에 영향 |

### 관리 흐름
1. **설정 단계**: `FTickFunction`에서 사용자 설정값 저장
2. **등록 단계**: Prerequisite 분석하여 `ActualStartTickGroup` 계산
3. **실행 단계**: `FInternalData`의 실제값으로 틱 실행

## 설계 철학

### 1. 관심사 분리 (Separation of Concerns)
- **FTickFunction**: 설정값 관리
- **FInternalData**: 런타임 상태 관리

### 2. 메모리 효율성
- 비트 필드 사용 (`bool bRegistered : 1`, `bool bWasInterval : 1`)
- 상대적 시간 계산으로 메모리 절약

### 3. 성능 최적화
- 연결 리스트를 통한 효율적인 쿨다운 관리
- 상대적 시간 계산으로 빠른 쿨다운 체크

### 4. 유연성
- Prerequisite에 따른 동적 그룹 조정 지원
- 런타임에서 틱 함수 상태 변경 가능

## 핵심 포인트

1. **이중 구조**: 설정과 실행 상태를 분리하여 관리
2. **동적 조정**: Prerequisite에 따른 실제 실행 그룹 자동 조정
3. **효율적 쿨다운**: 상대적 시간 계산을 통한 최적화
4. **시스템 통합**: 태스크 그래프와의 완전한 연동

