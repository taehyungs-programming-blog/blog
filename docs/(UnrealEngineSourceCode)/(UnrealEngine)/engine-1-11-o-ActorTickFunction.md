---
layout: default
title: "11. FActorTickFunction"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
// 우선, Actor내부에 존재한다.
struct FActorTickFunction PrimaryActorTick;
    // 대략 시작은 Activate(true)를 타면서 Tick이 시작되며 시작한다.
```

```cpp
struct FActorTickFunction : public FTickFunction
{
    /** AActor that is the target of this tick */
    class AActor* Target;
};
```

```cpp
struct FTickFunction
{
    // ...

    // tick을 돌릴지 여부
    uint8 bCanEverTick : 1;

    // 시작시 활성화 여부
    uint8 bStartWithTickEnabled : 1;

    // tick의 상태
    enum class ETickState : uint8
    {
        Disabled,
        Enabled,
        CoolingDown
    };
    ETickState TickState;


}
```

---

* 참고) ActorComponent::Activate

```cpp
virtual void Activate(bool bReset=false)
{
    if (bReset || ShouldActivate() == true)
    {
        SetComponentTickEnabled(true);
        
        SetActiveFlag(true);

        OnComponentActivated.Broadcast(this, bReset);
    }
}
```
