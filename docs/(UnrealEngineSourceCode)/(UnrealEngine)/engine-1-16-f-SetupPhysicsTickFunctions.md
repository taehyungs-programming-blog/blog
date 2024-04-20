---
layout: default
title: "15. Wrold::SetupPhysicsTickFunctions"
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
    void SetupPhysicsTickFunctions(float DeltaSeconds)
{
    StartPhysicsTickFunction.bCanEverTick = true;
    StartPhysicsTickFunction.Target = this;

    EndPhysicsTickFunction.bCanEverTick = true;
    EndPhysicsTickFunction.Target = this;

    bool bEnablePhysics = bShouldSimulatePhysics;

    bool bNeedToUpdateTickRegistration = (bEnablePhysics != StartPhysicsTickFunction.IsTickFunctionRegistered())
        || (bEnablePhysics != EndPhysicsTickFunction.IsTickFunctionRegistered());

    if (bNeedToUpdateTickRegistration && PersistentLevel)
    {
        if (bEnablePhysics && !StartPhysicsTickFunction.IsTickFunctionRegistered())
        {
            StartPhysicsTickFunction.TickGroup = TG_StartPhysics;
            StartPhysicsTickFunction.RegisterTickFunction(Persistent);
        }
        else if (!bEnablePhysics && StartPhysicsTickFunction.IsTickFunctionRegistered())
        {
            StartPhysicsTickFunction.UnRegisterTickFunction();
        }

        if (bEnablePhysics && !EndPhysicsTickFunction.IsTickingFunctionRegistered())
        {
            EndPhysicsTickFunction.TickGroup = TG_EndPhysics;
            EndPhysicsTickFunction.RegisterTickFunction(PersistentLevel);
            EndPhysicsTickFunction.AddPrerequisite(this, StartPhysicsTickFunction);
        }
        else if (!bEnablePhysics && EndPhysicsTickFunction.IsTickFunctionRegistered())
        {
            EndPhysicsTickFunction.RemovePrerequisite(this, StartPhysicsTickFunction);
            EndPhysicsTickFunction.UnRegisterTickFunction();
        }  
    }
}
```