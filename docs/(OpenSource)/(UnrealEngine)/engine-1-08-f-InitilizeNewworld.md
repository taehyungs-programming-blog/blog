---
layout: default
title: "08. InitializeNewWorld"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(OpenSource👽)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
// World.h

void InitializeNewWorld(const InitializationValues IVS = InitializationValues(), bool bInSkipInitWorld = false)
{
    // ...

    // PersistentLevel을 만들며 this를 넘긴다.
        // PersistentLevel - 현재 World와 생사주기를 같이하는 Level
        // 즉 PersistentLevel에 World(this)를 넘기고 PersistentLevel로 등록한다는말.
    PersistentLevel = NewObject<ULevel>(this, TEXT("PersistentLevel"));
    // 그리고 OwningWorld를 this로 둔다.
    PersistentLevel->OwningWorld = this;

    // ...

    // World의 세팅
        // Actor로 선언된건 World가 내려가기전 내려가면안되기 때문
    AWorldSettings* WorldSettings = nullptr;
    {
        // Actor는 보통 FActorSpawnParameters를 통해 Spawn
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        SpawnInfo.Name = GEngine->WorldSettingsClass->GetFName();

        WorldSettings = SpawnActor<AWorldSettings>(GEngine->WorldSettingsClass, SpawnInfo);
    }
```