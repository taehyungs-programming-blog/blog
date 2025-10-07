---
layout: default
title: "16. TickTaskLevel"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

# FTickTaskLevel 클래스 분석

## 개요

* `FTickTaskLevel`은 Unreal Engine의 Tick 시스템을 관리하는 핵심 클래스다.
* 이 클래스는 Level 단위에서 모든 Tick 함수들을 효율적으로 관리하며, Actor와 ActorComponent에 상관없이 통합된 Tick 관리 시스템을 제공

## 클래스 구조

```cpp
class FTickTaskLevel
{
    // Tick 자체는 Actor와 ActorComponent 상관 없이 Level에서 관리
    TSet<FTickFunction*> AllEnabledTickFunctions;
    TSet<FTickFunction*> AllDisabledTickFunctions;
    
    struct FTickScheduleDetails
    {
        FTickFunction* TickFunction;
        
        // 몇 초가 지난 후에 Enable로 변경될지에 대한 시간
        float Cooldown;
    };
    
    // TickInterval로 Tick에 주기를 설정하면 주기 오름차순대로 정렬하고 CoolingDown에 저장 
    TArrayWithThreadsafeAdd<FTickScheduleDetails> TickFunctionsToReschedule;
    
    struct FCoolingDownTickFunctionList
    {
        FTickFunction* Head;
    }
    
    // CoolingDown의 경우 LinkedList 형식으로 시간 주기 순서대로 연결하여 저장
    FCoolingDownTickFunctionList AllCoolingDownTickFunctions;
}
```

## 주요 구성 요소

### 1. Tick 함수 상태 관리

#### AllEnabledTickFunctions
- **타입**: `TSet<FTickFunction*>`
- **용도**: 현재 실행 중인 Tick 함수들을 저장
- **특징**: 해시 테이블 기반으로 O(1) 검색 성능

#### AllDisabledTickFunctions
- **타입**: `TSet<FTickFunction*>`
- **용도**: 비활성화된 Tick 함수들을 저장
- **특징**: 중복을 방지하고 빠른 검색/삭제가 가능

### 2. Tick 스케줄링 구조체

#### FTickScheduleDetails
```cpp
struct FTickScheduleDetails
{
    FTickFunction* TickFunction;  // Tick 함수 포인터
    float Cooldown;               // 재활성화까지 남은 시간(초)
};
```

- **목적**: Tick 함수와 해당 함수의 쿨다운 시간을 함께 관리
- **사용**: `TickInterval`이 설정된 Tick 함수들의 재스케줄링에 활용

### 3. 재스케줄링 대기열

#### TickFunctionsToReschedule
- **타입**: `TArrayWithThreadsafeAdd<FTickScheduleDetails>`
- **목적**: `TickInterval`로 설정된 Tick 함수들을 주기별로 재정렬
- **특징**: 
  - 스레드 안전한 추가 연산 지원
  - 쿨다운 시간 오름차순으로 정렬
  - 효율적인 스케줄링을 위한 임시 저장소

### 4. 쿨다운 Tick 함수 리스트

#### FCoolingDownTickFunctionList
```cpp
struct FCoolingDownTickFunctionList
{
    FTickFunction* Head;  // 연결 리스트의 헤드 포인터
};
```

- **구조**: LinkedList 형태로 구현
- **정렬**: 시간 주기 순서대로 연결
- **장점**: 시간 순서대로 효율적인 순회 가능

## Tick 관리 시스템의 동작 원리

### 1. Tick 함수 등록 과정

1. **초기 등록**: Actor나 ActorComponent가 생성될 때 Tick 함수가 등록됨
2. **상태 분류**: `AllEnabledTickFunctions` 또는 `AllDisabledTickFunctions`에 추가
3. **간격 설정**: `TickInterval`이 설정된 경우 `TickFunctionsToReschedule`에 추가

### 2. Tick 실행 주기 관리

#### 일반 Tick
- **실행 주기**: 매 프레임마다 실행
- **관리 위치**: `AllEnabledTickFunctions`에서 처리
- **특징**: 기본적인 Tick 동작

#### 간격 Tick
- **설정 방법**: `TickInterval` 설정
- **관리 시스템**: 쿨다운 시스템 사용
- **동작**: 지정된 간격으로만 실행

#### 쿨다운 관리
- **저장 위치**: `AllCoolingDownTickFunctions`
- **동작**: 시간 체크 후 재활성화
- **최적화**: LinkedList를 통한 효율적인 시간 관리

### 3. 성능 최적화 전략

- **TSet 활용**: O(1) 검색/삭제 성능으로 빠른 Tick 함수 관리
- **LinkedList 구조**: 쿨다운 Tick들의 순차적 처리로 메모리 효율성 증대
- **스레드 안전성**: 멀티스레드 환경에서 안전한 추가 연산 지원
- **정렬 최적화**: 시간 순서대로 정렬하여 불필요한 연산 최소화

## 데이터 구조별 상세 분석

### TSet<FTickFunction*>
- **기반 기술**: 해시 테이블
- **성능**: O(1) 검색, 삽입, 삭제
- **사용 시나리오**: 
  - Tick 함수의 활성화/비활성화 상태 변경
  - 빠른 Tick 함수 검색
- **장점**: 중복 방지, 빠른 접근

### TArrayWithThreadsafeAdd<FTickScheduleDetails>
- **기반 기술**: 동적 배열 + 스레드 안전 래퍼
- **특징**: 
  - 스레드 안전한 추가 연산
  - Cooldown 시간 오름차순 정렬
- **사용 시나리오**: 
  - 재스케줄링이 필요한 Tick 함수들의 임시 저장
  - 간격 Tick 함수들의 배치 처리

### FCoolingDownTickFunctionList (LinkedList)
- **구조**: 단방향 연결 리스트
- **정렬**: 시간 주기 순서대로 연결
- **장점**: 
  - 시간 순서대로 효율적인 순회
  - 메모리 효율적인 삽입/삭제
  - 동적 크기 조정
- **사용**: 쿨다운 중인 Tick 함수들의 시간 관리

## 시스템의 장점

### 1. 효율성
- 각 데이터 구조가 최적화된 용도로 사용
- 불필요한 Tick 실행 최소화
- 메모리 사용량 최적화

### 2. 확장성
- Level 단위 관리로 대규모 씬에서도 안정적
- 다양한 Tick 주기 설정 지원
- 동적 Tick 함수 추가/제거 지원

### 3. 유연성
- 일반 Tick과 간격 Tick 모두 지원
- 런타임 중 Tick 함수 상태 변경 가능
- 다양한 Tick 주기 설정 옵션

### 4. 성능
- 해시 테이블 기반 빠른 검색
- LinkedList를 통한 효율적인 시간 관리
- 스레드 안전한 동시성 처리
