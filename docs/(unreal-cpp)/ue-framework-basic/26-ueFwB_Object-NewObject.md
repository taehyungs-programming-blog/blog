---
layout: default
title: "26. NewObject"
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
Unreal Engine의 `NewObject`는 런타임에 UObject를 동적으로 생성하는 핵심 메커니

## 1. NewObject 사용 예시

### 기본 사용법
```cpp
void AFrameWorkCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // 이 시점에서 outer만 설정되어 있으면 OwnedComponents 추가됨
    UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(this);

    // (주의) Register는 직접 호출
    Mesh->RegisterComponent();
}
```

### 핵심 포인트
- **Outer 설정**: `this`를 Outer로 전달하여 소유권 관계 설정
- **OwnedComponents 자동 추가**: Outer가 설정되면 자동으로 소유 컴포넌트 목록에 추가
- **수동 Register**: 컴포넌트 활성화를 위해 `RegisterComponent()` 직접 호출 필요

## 2. NewObject 템플릿 함수 분석

```cpp
template<class T>
T* NewObject(UObject* Outer = (UObject*)GetTransientPackage())
{
    // 생성자에 NewObject는 불가능
    FObjectInitializer::AssertIfInConstructor()
    
    FStaticConstructObjectParameters Params(T::StaticClass());
    Params.Outer = Outer;

    T* Result = static_cast<T*>(StaticConstructObject_Internal(Params));
    return Result;
}
```

### 생성자에서 NewObject 사용 불가 이유

**핵심 이유**: 객체 생성 과정에서 순환 참조와 초기화 순서 문제 발생

1. **순환 참조 위험**
   - 생성자 내에서 NewObject 호출 시, 아직 완전히 초기화되지 않은 객체를 Outer로 사용
   - 이로 인해 가비지 컬렉션과 참조 관리에 문제 발생

2. **초기화 순서 문제**
   - UObject 생성자는 복잡한 초기화 과정을 거침
   - 생성자 내에서 다른 객체를 생성하면 초기화 순서가 꼬일 수 있음

3. **메모리 관리 문제**
   - 생성자 실행 중에는 객체가 불완전한 상태
   - 이 상태에서 다른 객체를 생성하면 메모리 누수나 크래시 위험

4. **가비지 컬렉션 문제**
   - UE의 가비지 컬렉터는 완전히 초기화된 객체만 안전하게 관리 가능
   - 생성자 내 생성 객체는 GC 추적이 어려움

## 3. StaticConstructObject_Internal 분석

```cpp
UObject* StaticConstructObject_Internal(const FStaticConstructObjectParameters& Params)
{
    const UClass* InClass = Params.Class;
    UObject* InOuter = Params.Outer;
    const FName& InName = Params.Name;
    
    UObject* Result = StaticAllocateObject(InClass, InOuter, InName, InFlags, Params.InternalSetFlags, bCanRecycleSubobjects, &bRecycledSubobject, Params.ExternalPackage);
    
    (*InClass->ClassConstructor)(FObjectInitializer(Result, Params));
}
```

### 프로세스 단계

1. **StaticAllocateObject**: 메모리 할당
   - C++의 `malloc`과 유사한 역할
   - UObject의 메모리 공간을 할당하지만 아직 초기화되지 않음

2. **ClassConstructor 호출**: 생성자 실행
   - **Placement New**: 이미 할당된 메모리에 생성자 호출
   - Unreal의 "흑마법"이라 불리는 이유: 메모리 관리와 생성자 호출의 복잡한 조합

## 4. FObjectInitializer 소멸자

```cpp
FObjectInitializer::~FObjectInitializer()
{
    PostConstructInit();
}
```

### 역할
- 생성자 실행 완료 후 후처리 작업 수행
- 객체의 최종 초기화 상태로 만드는 핵심 단계

## 5. PostConstructInit 분석

```cpp
void FObjectInitializer::PostConstructInit()
{
    Obj->PostInitProperties();
}
```

### 기능
- 객체의 `PostInitProperties()` 가상 함수 호출
- 각 클래스별 특화된 후처리 로직 실행

## 6. UActorComponent::PostInitProperties

```cpp
void UActorComponent::PostInitProperties()
{
    Super::PostInitProperties();
    
    if (OwnerPrivate && CreationMethod != EComponentCreationMethod::Instance)
    {
        OwnerPrivate->AddOwnedComponent(this);
    }
}
```

### 핵심 로직
- **Owner 확인**: `OwnerPrivate`가 존재하는지 확인
- **CreationMethod 체크**: Instance가 아닌 경우에만 추가
- **자동 소유권 설정**: Owner에 컴포넌트를 자동으로 추가

## 7. AActor::AddOwnedComponent

```cpp
void AActor::AddOwnedComponent(UActorComponent* Component)
{
    // 액터 컴포넌트 목록에 추가
    OwnedComponents.Add(Component, &bAlreadyInSet);
}
```

### 기능
- 컴포넌트를 Actor의 소유 컴포넌트 목록에 추가
- 중복 추가 방지를 위한 플래그 사용

## 주요 특징 요약

1. **메모리 관리**: StaticAllocateObject로 메모리 할당 후 Placement New로 생성자 호출
2. **소유권 자동 설정**: Outer가 설정되면 자동으로 소유 컴포넌트 목록에 추가
3. **생성자 제한**: 생성자 내에서 NewObject 사용 불가 (순환 참조 및 초기화 순서 문제)
4. **후처리 자동화**: PostConstructInit을 통한 자동 후처리
5. **타입 안전성**: 템플릿을 통한 타입 안전한 객체 생성

## 주의사항

- **생성자 내 사용 금지**: 반드시 BeginPlay() 등에서 사용
- **수동 Register**: 컴포넌트 활성화를 위해 RegisterComponent() 직접 호출 필요
- **메모리 관리**: UE의 가비지 컬렉션 시스템에 의존
- **초기화 순서**: PostInitProperties에서 추가 초기화 로직 구현 가능
