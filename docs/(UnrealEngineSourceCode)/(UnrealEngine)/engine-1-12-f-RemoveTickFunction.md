---
layout: default
title: "12. RemoveTickFunction"
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
// TickTaskManager.h

void RemoveTickFunction(FTickFunction* TickFunction)
{
    // TickState는 아래와 같이 있음
        // AllEnabledTickFunctions, 
        // AllDisabledTickFunctions, 
        // TickFunctionsToReschedule,
        // AllCoolingDownTickFunctions
    switch(TickFunction->TickState)
    {
    case FTickFunction::ETickState::Enabled:
        if (TickFunction->InternalData->bWasInternal)
            // 이 상태는 Tick은 돌고있는데 CoolingDown일 확률이 높음.
        
        // ...
}
```