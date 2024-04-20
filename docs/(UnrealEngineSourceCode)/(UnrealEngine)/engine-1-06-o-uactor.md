---
layout: default
title: "06. UActor"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* Actor는 Level에 존재한다.
* 각 Actor는 Actor Component를 보유한다.
    * Actor Component : UStaticMeshComponent, USkeletalMeshComponent ... 이 Actor Component이다.

```cpp
class AActor : public UObject
{
    // ...

    // 역시 멤버변수를 먼저 확인해 보자면 
    TSet<TObjectPtr<UActorComponent>> OwnedComponents;
        // 컴포넌트 관리
        // ??? -> 그런데 신기하지 않는가? 내가 알기로 Component는 Hierarchy 구조를 갖는다

    // ...
}
```

* 아래와 같이 Hierarchy를 갖는다
    * 이걸 어떻게 TSet으로 관리하지??

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/opensource/unreal/engine-1-6-1.png"/>
</p>

```cpp
// ...

// 답부터 말하면 RootComponent 아래 Component를 갖게 한다.
TObjectPtr<USceneComponent> RootComponent;
```

```
RootComponent [ Component 0 ]
    |
    ----------- Component 1
    |
    ----------- Component 2
                     |
                     ---------- Component 3
```

* `Component 1, 2`의 Parent Component는 `Component 0`이다.
* `Component 3`의 Parent Component는 `Component 2`이다.
* 단순 자신의 Parent를 Parent로 붙이지 않는 이유는 상대경로를 특정 Actor로 지정하기위함이다.

```cpp
// Hierarchy 구조를 좀 어렵게 Scene-Graph구조라 한다.
class USceneComponent : public UActorComponent
{
    // 내부 변수가 Parent, Children 구조임을 알 수
    UPROPERTY(ReplicatedUsing=OnRep_AttachParent)
    TObjectPtr<USceneComponent> AttachParent;

    UPROPERTY(ReplicatedUsing = OnRep_AttachChildren, Transient)
    TArray<TObjectPtr<USceneComponent>> AttachChildren;
}
```