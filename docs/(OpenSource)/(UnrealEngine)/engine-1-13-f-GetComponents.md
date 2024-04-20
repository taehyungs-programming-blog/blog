---
layout: default
title: "13. GetComponents"
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
template <class AllocatorType>
void GetComponents(TArray<UActorComponent*, AllocatorType>& OutComponents, bool bIncludeFromChildActors = false)
{
    // 리셋
    OutComponents.Reset();
    
    // 순회하며 OutComponents에 주입
    ForEachComponent_Internal<UActorComponent>(UActorComponent::StaticClass(), bIncludeFromChildActor, [&](UActorComponent* InComp)
    {
        OutComponents.Add(InComp);
    });
}
```

```cpp
template <class ComponentType, typename Func>
void ForEachComponent_Internal(TSubclassOf<UActorComponent> ComponentClass, bool bIncludeFromChildActors, Func InFunc) const
{
    if (ComponentClass == UActorComponent::StaticClass())
    {
        // 아래 처럼 true, true, false, false넣는 이유는 컴파일 타임에 결정나게 하기 위함.
        if (bIncludeFromChildActors)
        {
            ForEachComponent_Internal<ComponentType, true /*bClassIsActorComponent*/, true /*bIncludeFromChildActors*/>(ComponentClass, InFunc);
        }
        else
        {
            ForEachComponent_Internal<ComponentType, true /*bClassIsActorComponent*/, false /*bIncludeFromChildActors*/>(ComponentClass, InFunc);
        }
    }
    else
    {
        if (bIncludeFromChildActors)
        {
            ForEachComponent_Internal<ComponentType, false /*bClassIsActorComponent*/, true /*bIncludeFromChildActors*/>(ComponentClass, InFunc);
        }
        else
        {
            ForEachComponent_Internal<ComponentType, false /*bClassIsActorComponent*/, false /*bIncludeFromChildActors*/>(ComponentClass, InFunc);
        }
    }
}

```

```cpp
// 그래서 어떻게 순회하냐?
template <class ComponentType, bool bClassIsActorComponent, bool bIncludeFromChildActors, typename Func>
void ForEachComponent_Internal(TSubclassOf<UActorComponent> ComponentClass, Func InFunc) const
{
    check(bClassIsActorComponent == false || ComponentClass == UActorComponent::StaticClass());
    check(ComponentClass->IsChildOf(ComponentType::StaticClass()));

    if (bIncludeFromChildActors)
    {
        TArray<AActor*, TInlineAllocator<NumInlinedActorComponents>> ChildActors;
        for (UActorComponent* OwnedComponent : OwnedComponents)
        {
            if (OwnedComponent)
            {
                if (bClassIsActorComponent || OwnedComponent->IsA(ComponentClass))
                {
                    InFunc(static_cast<ComponentType*>(OwnedComponent));
                }

                if (UChildActorComponent* ChildActorComponent = Cast<UChildActorComponent>(OwnedComponent))
                {
                    if (AActor* ChildActor = ChildActorComponent->GetChildActor())
                    {
                        ChildActors.Add(ChildActor);
                    }
                }
            }
        }

        for (AActor* ChildActor : ChildActors)
        {
            ChildActor->ForEachComponent_Internal<ComponentType, bClassIsActorComponent, bIncludeFromChildActors>(ComponentClass, InFunc);
        }
    }
    else
    {
        for (UActorComponent* OwnedComponent : OwnedComponents)
        {
            if (OwnedComponent)
            {
                if (bClassIsActorComponent || OwnedComponent->IsA(ComponentClass))
                {
                    InFunc(static_cast<ComponentType*>(OwnedComponent));
                }
            }
        }
    }
}
```