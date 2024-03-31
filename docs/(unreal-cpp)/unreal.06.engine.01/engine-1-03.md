---
layout: default
title: "03. UEngine"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* UEngine의 자녀인 UEditorEngine를 살펴보자

## UEngine

```cpp
class UEngine : public UObject, public FExec
// class UEditorEngine : public UEngine
{
    // ...

    // Engine에선 WorldList를 별도로 관리한다.
        // --> World의 생성/제거의 주체는 Engine이다.
    TIndirectArray<FWorldContext> WorldList;
    int32 NextWorldContextHandle;
}
```

* `UObject` - GC에 의해  LifeTime이 결정

## FWorldContext

* UEngine과 World의 Dependency를 끊기위해 사용됨.

```cpp
struct FWorldContext
{
    void SetCurrentWorld(UWorld* World)
    {
        UWorld* OldWorld = ThisCurrentWorld;
        ThisCurrentWorld = World;

        if (OwningGameInstance)
        {
            OwningGameInstance->OnWorldChanged(OldWorld, ThisCurrentWorld);
        }
    }

    TObjectPtr<UWorld> ThisCurrentWorld;
    // ...
```

* 예를들어 보자면 ..

```cpp
class UEditorEngine : public UEngine
{
public:
    // UEditorEngine에서 이런식으로 WorldContext를 받아온다
    FWorldContext& GetEditorWorldContext(bool bEnsureIsGWorld = false)
```

```cpp
// 실 사용은 이렇게 ..
UWorld* World = GEditor->GetEditorWorldContext().World();
/*
FORCEINLINE UWorld* World() const
{
    return ThisCurrentWorld;
}
*/
```

---
---

## TIPS) `: 1` 과 같은 표현은 뭘 의미할까?

```cpp
// EX

typedef struct _CUVIDVP9PICPARAMS
{
    // ...

    // short를 모두 쓰지않고 1비트만 쓴다를 의미
    unsigned short frameType : 1;
    unsigned short showFrame : 1;

    // 이런경우 3비트겠지?
    unsigned short profile : 3;
    unsigned short frameContextIdx : 2;
```

---

## `FActorTickFunction`

