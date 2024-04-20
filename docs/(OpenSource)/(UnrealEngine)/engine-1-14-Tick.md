---
layout: default
title: "14. Tick"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(OpenSource👽)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 여기서 부턴 Tick에 대해서 다룹니다.

```cpp
virtual void Tick(float DeltaSeconds, bool bIdleMode) override
{
    UWorld* CurrentGWorld = GWorld;

    // 지난 강의까진 GetEditorWorldContext에 대해 다뤘다
    FWorldContext& EditorContext = GetEditorWorldContext();
    
    // 이제 여기부터 시작!
    bool bShouldTickEditorWorld = true;
    ELevelTick TickType = IsRealTime ? LEVELTICK_ViewportOnly : LEVELTICK_TimeOnly;
    
    // 여기는 Editor에서만 동작.
    if (bShouldTickEditorWorld)
    {
        {
            EditorContext.World()->Tick(TickType, DeltaSeconds);
        }
    }

    {
        TArray<FWorldContext*> LocalPieContextPtrs;
        for (FWorldContext& PieContext : WorldList)
        {
            // PIE Context를 찾아서
            if (PieContext.WorldType == EWorldType::PIE && PieContext.World() != nullptr)
            {
                // LocalPieContextPtrs에 넣는다
                LocalPieContextPtrs.Add(&PieContext);
            }
        }

        for (FWorldContext* PieContextPtr : LocalPieContextPtrs)
        {
            FWorldContext& PieContext = *PieContextPtr;
            PlayWorld = PieContext.World();
            GameViewport = PieContext.GameViewport;

            float TickDeltaSeconds;
            if (PieContext.PIEFixedTickSeconds > 0.f)
            {
                PieContext.PIEAccumulatedTickSeconds += DeltaSeconds;
                TickDeltaSeconds = PieContext.PIEFixedTickSeconds;
            }
            else
            {
                PieContext.PIEAccumulatedTickSeconds = DeltaSeconds;
                TickDeltaSeconds = DeltaSeconds;
            }

            for (; PieContext.PIEAccumulatedTickSeconds >= TickDeltaSeconds; PieContext.PIEAccumulatedTickSeconds -= TickDeltaSeconds)
            {
                {
                    // 결국 여기로 들어간다.
                    PieContext.World()->Tick(LEVELTICK_All, TickDeltaSeconds);
                }
            }
        }
    }
}
```