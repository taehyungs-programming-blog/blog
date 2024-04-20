---
layout: default
title: "07. USubsystem"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 우선, `USubsystem`을 왜 쓸까?
    * LifeTiem을 특정 Component와 맞추고 싶다
    * 그건 Delegate로 이미 만들지 않았나?
        * 그 Delegate를 쓰지않고 LiftTime을 맞추고 싶다가 목적
    * 그럼 특정 Component엔 뭐가 있지?
        * Engine (ex. UEngineSubsystem을 상속해 쓰면 됨)
        * Editor
        * GameInstance
        * World
        * LocalPlayer
        * 흠, 모두 중요한애들 뿐이군!

```cpp
class UWorldSubsystem : public USubsystem
{/* ... */}
// ...

class USubsystem : public UObject
{
    // ...

    virtual void Initialize(FSubsystemCollectionBase& Collection) {}
    virtual void Deinitialize() {}

    // ...
}
```
