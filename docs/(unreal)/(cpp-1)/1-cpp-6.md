---
layout: default
title: "06. 무기 충돌 개선"
parent: "(Unreal C++ RPG Action만 구현)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 하려고 하는건 공격할때만 충돌체크하는 것이다.
* 이게 필요한 이유가 이걸안하면 그냥 무기옆에만 서도 충돌되어 피격처리된다.

```cpp
//PlayerWeapon

WeaponCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponCollision"));

WeaponCollision->InitSphereRadius(10.0f);

// 초기는 충돌체크를 안한다
WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

FName WeaponSocket = FName(TEXT("WeaponPivotSocket"));

WeaponCollision->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
```

```cpp
// Creature

SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

SphereComponent->InitSphereRadius(20.0f);

// 초기는 충돌체크를 안한다
SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

FName WeaponSocket = FName(TEXT("RightHandSocket"));

SphereComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
```

* 애니메이션이 Notify를 추가하자

<사진1>

<사진2>

* 추가로 피격시 카메라가 흔들리는 버그가 있는데

<사진3>

* Do Collision Test를 끄면 해결된다

---

* 오직 C++로만 해결해 보자

```cpp
UFUNCTION()
void AnimNotify_SetInitCollision();

UFUNCTION()
void AnimNotify_SetWeaponCollisionEnabled();

UFUNCTION()
void AnimNotify_SetWeaponCollisionDisabled();
```
