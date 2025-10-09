---
layout: default
title: "22. TickTask와 TickGroup"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 배정 과정 단계

### 1. 활성화된 TickFunction 순회 및 QueueTickFunction 배정
```cpp
// 활성화된 모든 TickFunction을 순회
for (FTickFunction* TickFunction : ActiveTickFunctions)
{
    if (TickFunction->IsTickFunctionEnabled())
    {
        // QueueTickFunction에 추가
        QueueTickFunction(TickFunction);
    }
}
```

### 2. 선행조건(Prerequisites) 확인 및 우선 배정
```cpp
// 선행조건이 있는 TickFunction들을 먼저 처리
for (FTickFunction* TickFunction : QueuedTickFunctions)
{
    if (TickFunction->HasPrerequisites())
    {
        // 선행조건들을 먼저 배정
        ProcessPrerequisites(TickFunction);
    }
}
```

### 3. MaxPrerequisiteTickGroup 계산
선행조건들 중 가장 늦은 TickGroup을 찾는 과정입니다.

#### 예시 1: 단순한 선행조건
```cpp
// TickFunction A가 TickFunction B를 선행조건으로 가질 때
FTickFunction* FunctionA = new FTickFunction();
FTickFunction* FunctionB = new FTickFunction();

FunctionA->Prerequisites.Add(FunctionB);
FunctionB->TickGroup = TG_PrePhysics;  // Group 0

// FunctionA의 MaxPrerequisiteTickGroup = TG_PrePhysics (0)
```

#### 예시 2: 복잡한 선행조건
```cpp
// TickFunction C가 여러 선행조건을 가질 때
FTickFunction* FunctionC = new FTickFunction();
FTickFunction* FunctionD = new FTickFunction(); // TG_PrePhysics (0)
FTickFunction* FunctionE = new FTickFunction(); // TG_DuringPhysics (1)
FTickFunction* FunctionF = new FTickFunction(); // TG_PostPhysics (2)

FunctionC->Prerequisites.Add(FunctionD);
FunctionC->Prerequisites.Add(FunctionE);
FunctionC->Prerequisites.Add(FunctionF);

// MaxPrerequisiteTickGroup = TG_PostPhysics (2) - 가장 늦은 그룹
```

#### 예시 3: 중첩된 선행조건
```cpp
// A -> B -> C -> D 체인
FTickFunction* FunctionA = new FTickFunction(); // TG_PrePhysics (0)
FTickFunction* FunctionB = new FTickFunction(); // TG_DuringPhysics (1)
FTickFunction* FunctionC = new FTickFunction(); // TG_PostPhysics (2)
FTickFunction* FunctionD = new FTickFunction(); // TG_PostUpdateWork (3)

// D의 선행조건: A, B, C
FunctionD->Prerequisites.Add(FunctionA);
FunctionD->Prerequisites.Add(FunctionB);
FunctionD->Prerequisites.Add(FunctionC);

// MaxPrerequisiteTickGroup = TG_PostPhysics (2)
```

### 4. 최종 TickGroup 결정
자신의 TickGroup과 MaxPrerequisiteTickGroup 중 더 늦은 그룹을 선택합니다.

```cpp
ETickingGroup CalculateFinalTickGroup(FTickFunction* TickFunction)
{
    ETickingGroup MyTickGroup = TickFunction->TickGroup;
    ETickingGroup MaxPrerequisiteTickGroup = CalculateMaxPrerequisiteTickGroup(TickFunction);
    
    // 더 늦은 TickGroup을 선택
    ETickingGroup FinalTickGroup = (ETickingGroup)FMath::Max((int32)MyTickGroup, (int32)MaxPrerequisiteTickGroup);
    
    return FinalTickGroup;
}
```

#### 예시: 최종 TickGroup 결정
```cpp
// Case 1: 자신의 그룹이 더 늦은 경우
FTickFunction* Function1 = new FTickFunction();
Function1->TickGroup = TG_PostPhysics;        // 2
Function1->MaxPrerequisiteTickGroup = TG_PrePhysics; // 0
// FinalTickGroup = TG_PostPhysics (2)

// Case 2: 선행조건 그룹이 더 늦은 경우
FTickFunction* Function2 = new FTickFunction();
Function2->TickGroup = TG_PrePhysics;         // 0
Function2->MaxPrerequisiteTickGroup = TG_PostPhysics; // 2
// FinalTickGroup = TG_PostPhysics (2)

// Case 3: 동일한 그룹인 경우
FTickFunction* Function3 = new FTickFunction();
Function3->TickGroup = TG_DuringPhysics;      // 1
Function3->MaxPrerequisiteTickGroup = TG_DuringPhysics; // 1
// FinalTickGroup = TG_DuringPhysics (1)
```

### 5. TickTask로 Wrapping
선행조건과 자신을 묶어서 TickTask로 래핑합니다.

```cpp
class FTickTask
{
public:
    TArray<FTickFunction*> Prerequisites;
    FTickFunction* TargetFunction;
    ETickingGroup AssignedTickGroup;
    
    FTickTask(FTickFunction* InFunction, ETickingGroup InTickGroup)
        : TargetFunction(InFunction)
        , AssignedTickGroup(InTickGroup)
    {
        // 선행조건들을 복사
        Prerequisites = InFunction->Prerequisites;
    }
    
    void Execute()
    {
        // 선행조건들 먼저 실행
        for (FTickFunction* Prereq : Prerequisites)
        {
            Prereq->ExecuteTick();
        }
        
        // 자신 실행
        TargetFunction->ExecuteTick();
    }
};
```

### 6. TickTaskSequencer에 배정
최종적으로 TickTask를 TickTaskSequencer에 배정합니다.

```cpp
class FTickTaskSequencer
{
private:
    TArray<TArray<FTickTask*>> TickGroups; // 각 TickGroup별로 Task 배열
    
public:
    void AssignTickTask(FTickTask* TickTask)
    {
        ETickingGroup Group = TickTask->AssignedTickGroup;
        
        // TickGroup에 맞는 배열에 추가
        if (Group >= TickGroups.Num())
        {
            TickGroups.SetNum(Group + 1);
        }
        
        TickGroups[Group].Add(TickTask);
    }
    
    void ExecuteAllTickGroups()
    {
        // 각 TickGroup을 순서대로 실행
        for (int32 GroupIndex = 0; GroupIndex < TickGroups.Num(); ++GroupIndex)
        {
            TArray<FTickTask*>& GroupTasks = TickGroups[GroupIndex];
            
            // 같은 그룹 내에서는 병렬 실행 가능
            for (FTickTask* Task : GroupTasks)
            {
                Task->Execute();
            }
        }
    }
};
```

## 전체 배정 과정 예시

```cpp
void ProcessTickGroupAssignment()
{
    FTickTaskSequencer Sequencer;
    
    // 1. 활성화된 TickFunction들 수집
    TArray<FTickFunction*> ActiveFunctions = CollectActiveTickFunctions();
    
    for (FTickFunction* Function : ActiveFunctions)
    {
        // 2. 선행조건 처리
        ProcessPrerequisites(Function);
        
        // 3. MaxPrerequisiteTickGroup 계산
        ETickingGroup MaxPrereqGroup = CalculateMaxPrerequisiteTickGroup(Function);
        
        // 4. 최종 TickGroup 결정
        ETickingGroup FinalGroup = (ETickingGroup)FMath::Max(
            (int32)Function->TickGroup, 
            (int32)MaxPrereqGroup
        );
        
        // 5. TickTask로 래핑
        FTickTask* TickTask = new FTickTask(Function, FinalGroup);
        
        // 6. TickTaskSequencer에 배정
        Sequencer.AssignTickTask(TickTask);
    }
    
    // 모든 배정이 완료되면 실행
    Sequencer.ExecuteAllTickGroups();
}
```

## TickGroup 우선순위

Unreal Engine의 기본 TickGroup 순서:
- `TG_PrePhysics` (0) - 물리 시뮬레이션 전
- `TG_DuringPhysics` (1) - 물리 시뮬레이션 중
- `TG_PostPhysics` (2) - 물리 시뮬레이션 후
- `TG_PostUpdateWork` (3) - 업데이트 작업 후
- `TG_LastDemotable` (4) - 마지막 그룹

## 주의사항

1. **순환 참조 방지**: 선행조건이 순환 참조를 만들지 않도록 주의
2. **성능 고려**: 너무 많은 선행조건은 성능에 영향을 줄 수 있음
3. **그룹 분산**: 적절한 TickGroup 분산으로 병렬 처리 효율성 확보

