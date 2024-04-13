---
layout: default
title: "04. UWorld"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(OpenSource👽)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UObject

* `UWorld`에 들어가기전 UObject의 이해가 필요하다.

```cpp
class UObject : public UObjectBaseUtility
{
    /*
    UObjectBaseUtility - UObject와 관련된 Utiltity만 보유
    class UObjectBaseUtility : public UObjectBase
    */

    // ...
};
```

```cpp
class UObjectBase
{
    // Flags 정보
        // 대표적 예시로 RF_Transient (File로 저장하지마) 등이 있다.
    EObjectFlags ObjectFlags;

    // 어떤 Package(File)에 저장되어 있는지 정보를 담는다
        // 주의할 점은 UE가 발전하며 항상 Package(File)이 아닐수 있다는 점이다.
    UObject* OuterPrivate;
};
```

---

## UWorld 시작

```cpp
class UWorld final : public UObject, public FNetworkNotify
{

    //  ...

    // 우선 UWorld가 관리하는 멤버변수를 먼저 보자.
    FURL URL;   // 관리중인 리소스의 주소
    TEnumAsByte<EWorldType::Type> WorldType;    // Game, Editor, PIE ...
        // Tip) TEnumAsByte는 Enum의 Bit연산을 하게 해주기위한 Wrapper이다.
    TObjectPtr<class ULevel> PersistentLevel;
        // ULevel도 중요한 개념! 다음강 참조!
        // PersistentLevel의 중요 기능중 하나는 WorldSetting을 갖는다 이다.
}
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

