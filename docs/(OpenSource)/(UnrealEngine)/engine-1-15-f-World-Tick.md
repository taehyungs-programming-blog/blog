---
layout: default
title: "14. Wrold::Tick"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(OpenSource👽)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
void Tick(ELevelTick TickType, float DeltaSeconds)
{
    // Delegation
    FWorldDelegates::OnWorldTickStart.Broadcast(this, TickType, DeltaSeconds);

    bool bDoingActorTicks = 
        (TickType != LEVELTICK_TimeOnly);

    bool bValidateLevelList = false;
    for (const FLevelCollection& LevelCollection : LevelCollections)
    {
        if (LevelCollection.GetType() != ELevelCollectionType::DynamicSourceLevels)
        {
            const int32 NumLevels = LevelCollection.GetLevels().Num();
            if (NumLevels != 0)
            {
                bValidateLevelList = true;
                break;
            }
        }
    }

    for (int32 i = 0; i < LevelCollections.Num(); ++i)
    {
        TArray<ULevel*> LevelsToTick;
        for (ULevel* CollectionLevel : LevelCollections[i].GetLevels())
        {
            const bool bAddToTickList = (bValidateLevelList == false) || Levels.Contains(CollectionLevel);
            if (bAddToTickList && CollectionLevel)
            {
                LevelsToTick.Add(CollectionLevel);
            }
        }

        FScopedLevelCollectionContextSwitch LevelContext(i, this);

        const bool bShouldSkipTick = (LevelsToTick.Num() == 0);
        if (bDoingActorTicks && !bShouldSkipTick)
        {
            SetupPhysicsTickFunctions(DeltaSeconds);

            TickGroup = TG_PrePhysics; 

            FTickTaskManagerInterface::Get().StartFrame(this, DeltaSeconds, TickType, LevelsToTick);

            {
                RunTickGroup(TG_PrePhysics);
            }
            {
                RunTickGroup(TG_StartPhysics);
            }
            {
                RunTickGroup(TG_DuringPhysics, false);
            }
            {
                TickGroup = TG_EndPhysics;
                RunTickGroup(TG_EndPhysics);
            }
            {
                RunTickGroup(TG_PostPhysics);
            }
        }

        // various ticks:
        // 1. LatentActionManager
        // 2. TimerManager
        // 3. TickableGameObjects
        // 4. CameraManager
        // 5. streaming volume

        if (bDoingActorTicks)
        {
            RunTickGroup(TG_PostUpdateWork);
            RunTickGroup(TG_LastDemotable);
        }

        FTickTaskManagerInterface::Get().EndFrame();
    }

    FWorldDelegates::OnWorldTickEnd.Broadcast(this, TickType, DeltaSeconds);
}
```