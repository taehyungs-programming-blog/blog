---
layout: default
title: "10. UpdateWorldComponents"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UWorld::UpdateWorldComponents

```cpp
void UpdateWorldComponents(bool bRerunConstructionScripts, bool bCurrentLevelOnly, FRegisterComponentContext* Context = nullptr)
{
    if (!IsRunningDedicatedServer())
    {
        // SubSystem에 안달고 NewObject를 하는이유는 GC에 걸리지 않게 방어를 하기 위함.
        if (!LineBatcher)
        {
            LineBatcher = NewObject<ULineBatchComponent>();
            LineBatcher->bCalculateAccurateBounds = false;
        }

        if(!LineBatcher->IsRegistered())
        {	
            // see UActorComponent::RegisterComponentWithWorld (goto 39)
            LineBatcher->RegisterComponentWithWorld(this, Context);
        }
}
``` 