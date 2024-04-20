---
layout: default
title: "17. TickTaskManager::StartFrame"
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
void StartFrame()
{
    // ## WaitForCleanup
    WaitForCleanup();

    for (int32 Index = 0; Index < TG_MAX; Index++)
    {
        // Reset을 하며 모든 Task를 비워준다.
        TickCompletionEvents[Index].Reset();
        for (int32 IndexInner = 0; IndexInner < TG_MAX; IndexInner++)
        {
            TickTasks[Index][IndexInner].Reset();
            HiPriTickTasks[Index][IndexInner].Reset();
        }
    }

    WaitForTickGroup = (ETickingGroup)0;
}
```

---

## WaitForCleanup

```cpp
void WaitForCleanup()
{
    // CleanupTasks가 남아있다면 대기하여 Clean.
        // Future를 대기한다 생각.
    if (CleanupTasks.Num() > 0)
    {
        FTaskGraphInterface::Get().WaitUntilTaskComplete(CleanupTasks, ENamedThreads::GameThread);
        CleanupTasks.Reset();
    }
}
```

---

