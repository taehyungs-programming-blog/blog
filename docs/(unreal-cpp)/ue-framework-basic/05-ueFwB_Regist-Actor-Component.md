---
layout: default
title: "05. Actor의 Component 등록"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
// 두 과정이 필요하다.
void ULevel::IncrementalUpdateComponents(int32 NumComponentsToUpdate, bool bRerunConstructionScripts, FRegisterComponentContext* Context)
{
    // ...

	do
	{
		switch (IncrementalComponentState)
		{
		case EIncrementalComponentState::Init:
			// Sort 
			SortActorsHierarchy(Actors, this);
			
            // ...

		case EIncrementalComponentState::RegisterInitialComponents:

            // Regist
			if (IncrementalRegisterComponents(true, NumComponentsToUpdate, Context))

            // ...
```

---

## SortActorsHierarchy

```cpp
static void SortActorsHierarchy(TArray<TObjectPtr<AActor>>& Actors, ULevel* Level)
{
	const double StartTime = FPlatformTime::Seconds();

	TMap<AActor*, int32> DepthMap;
	TArray<AActor*, TInlineAllocator<10>> VisitedActors;

	DepthMap.Reserve(Actors.Num());

	bool bFoundWorldSettings = false;

	// This imitates the internals of GetDefaultBrush() without the sometimes problematic checks
	ABrush* DefaultBrush = (Actors.Num() >= 2 ? Cast<ABrush>(Actors[1]) : nullptr); 
	TFunction<int32(AActor*)> CalcAttachDepth = [&DepthMap, &VisitedActors, &CalcAttachDepth, &bFoundWorldSettings, DefaultBrush](AActor* Actor)
	{
        // 정렬하려는 Actor의 Depth가 있다면 등록
		int32 Depth = 0;
		if (int32* FoundDepth = DepthMap.Find(Actor))
		{
			Depth = *FoundDepth;
		}
		else
		{
            // Depth가 없다면 찾는다
			
			if (Actor->IsA<AWorldSettings>())
			{
				if (!bFoundWorldSettings)
				{
                    // World Setting의 경우 Lowest로
					Depth = TNumericLimits<int32>::Lowest();
					bFoundWorldSettings = true;
				}

                // ...
			}
            // 찾으려는 Actor의 Parent가 있다면
			else if (AActor* ParentActor = Actor->GetAttachParentActor())
			{
                // 그리고 그 Parent를 Visit했다면
				if (VisitedActors.Contains(ParentActor))
				{
					FString VisitedActorLoop;
					for (AActor* VisitedActor : VisitedActors)
					{
						VisitedActorLoop += VisitedActor->GetName() + TEXT(" -> ");
					}
					VisitedActorLoop += Actor->GetName();

					UE_LOG(LogLevel, Warning, TEXT("Found loop in attachment hierarchy: %s"), *VisitedActorLoop);
					// Once we find a loop, depth is mostly meaningless, so we'll treat the "end" of the loop as 0
				}
				else
				{
					VisitedActors.Add(Actor);

					if (ParentActor->IsChildActor())
					{
						Depth = CalcAttachDepth(ParentActor) - 1;
					}
					else
					{
						Depth = CalcAttachDepth(ParentActor) + 1;
					}
				}
			}
			DepthMap.Add(Actor, Depth);
		}
		return Depth;
	};

    // 이렇게 모든 Actor를 순회하며 DepthMap을 만든다.
	for (AActor* Actor : Actors)
	{
		if (Actor)
		{
			CalcAttachDepth(Actor);
			VisitedActors.Reset();
		}
	}

    // ...

    // 그냥 버블 정렬이라 생각하자
	auto DepthSorter = [&DepthMap](AActor* A, AActor* B)
	{
		const int32 DepthA = A ? DepthMap.FindRef(A) : MAX_int32;
		const int32 DepthB = B ? DepthMap.FindRef(B) : MAX_int32;
		return DepthA < DepthB;
	};


	StableSortInternal(Actors.GetData(), Actors.Num(), DepthSorter);
	

	// ...

	// nullptr을 찾아 그 이후를 모두 날린다.
	int32 RemoveAtIndex = Actors.Num();
	while (RemoveAtIndex > 0 && Actors[RemoveAtIndex - 1] == nullptr)
	{
		--RemoveAtIndex;
	}

	if (RemoveAtIndex < Actors.Num())
	{
		Actors.RemoveAt(RemoveAtIndex, Actors.Num() - RemoveAtIndex);
	}
}
```

---

## IncrementalRegisterComponents

```cpp
bool ULevel::IncrementalRegisterComponents(bool bPreRegisterComponents, int32 NumComponentsToUpdate, FRegisterComponentContext* Context)
{
	// Find next valid actor to process components registration

	if (OwningWorld)
	{
		OwningWorld->SetAllowDeferredPhysicsStateCreation(true);
	}

	while (CurrentActorIndexForIncrementalUpdate < Actors.Num())
	{
		AActor* Actor = Actors[CurrentActorIndexForIncrementalUpdate];
		bool bAllComponentsRegistered = true;
		if (IsValid(Actor))
		{
			if (!Actor->HasActorRegisteredAllComponents() || GOptimizeActorRegistration == 0)
			{
#if PERF_TRACK_DETAILED_ASYNC_STATS
				FScopeCycleCounterUObject ContextScope(Actor);
#endif
				if (bPreRegisterComponents && !bHasCurrentActorCalledPreRegister)
				{
					Actor->PreRegisterAllComponents();
					bHasCurrentActorCalledPreRegister = true;
				}
                // 결국은 Actor->IncrementalRegisterComponents 여기를 통해서 Register
				bAllComponentsRegistered = Actor->IncrementalRegisterComponents(NumComponentsToUpdate, Context);
			}
#if !UE_BUILD_SHIPPING
			else if (GOptimizeActorRegistration == 2)
			{
				// Verify that there aren't any leftover components
				Actor->ForEachComponent(false, [](UActorComponent* Component)
				{
					ensureMsgf(Component->IsRegistered() || !Component->bAutoRegister, TEXT("Component %s should be registered!"), *Component->GetPathName());
				});
			}
#endif
		}

		if (bAllComponentsRegistered)
		{
			// All components have been registered for this actor, move to a next one
			CurrentActorIndexForIncrementalUpdate++;
			bHasCurrentActorCalledPreRegister = false;
		}

		// If we do an incremental registration return to outer loop after each processed actor 
		// so outer loop can decide whether we want to continue processing this frame
		if (NumComponentsToUpdate != 0)
		{
			break;
		}
	}

	if (CurrentActorIndexForIncrementalUpdate >= Actors.Num())
	{
		// We need to process pending adds prior to rerunning the construction scripts, which may internally
		// perform removals / adds themselves.
		if (Context)
		{
			Context->Process();
		}
		CurrentActorIndexForIncrementalUpdate = 0;
		return true;
	}

	return false;
}
```

```cpp
bool AActor::IncrementalRegisterComponents(int32 NumComponentsToRegister, FRegisterComponentContext* Context)
{
	if (NumComponentsToRegister == 0)
	{
		// 0 - means register all components
		NumComponentsToRegister = MAX_int32;
	}

	UWorld* const World = GetWorld();
	check(World);

	// If we are not a game world, then register tick functions now. If we are a game world we wait until right before BeginPlay(),
	// so as to not actually tick until BeginPlay() executes (which could otherwise happen in network games).
	if (bAllowTickBeforeBeginPlay || !World->IsGameWorld())
	{
		RegisterAllActorTickFunctions(true, false); // components will be handled when they are registered
	}
	
	// Register RootComponent first so all other children components can reliably use it (i.e., call GetLocation) when they register
	if (RootComponent != nullptr && !RootComponent->IsRegistered())
	{
#if PERF_TRACK_DETAILED_ASYNC_STATS
		FScopeCycleCounterUObject ContextScope(RootComponent);
#endif
		if (RootComponent->bAutoRegister)
		{
			// Before we register our component, save it to our transaction buffer so if "undone" it will return to an unregistered state.
			// This should prevent unwanted components hanging around when undoing a copy/paste or duplication action.
			RootComponent->Modify(false);

			RootComponent->RegisterComponentWithWorld(World, Context);
		}
	}

	int32 NumTotalRegisteredComponents = 0;
	int32 NumRegisteredComponentsThisRun = 0;
	TInlineComponentArray<UActorComponent*> Components;
	GetComponents(Components);
	TSet<UActorComponent*> RegisteredParents;
	
	for (int32 CompIdx = 0; CompIdx < Components.Num() && NumRegisteredComponentsThisRun < NumComponentsToRegister; CompIdx++)
	{
		UActorComponent* Component = Components[CompIdx];
		if (!Component->IsRegistered() && Component->bAutoRegister && IsValidChecked(Component))
		{
			// Ensure that all parent are registered first
			USceneComponent* UnregisteredParentComponent = GetUnregisteredParent(Component);
			if (UnregisteredParentComponent)
			{
				bool bParentAlreadyHandled = false;
				RegisteredParents.Add(UnregisteredParentComponent, &bParentAlreadyHandled);
				if (bParentAlreadyHandled)
				{
					UE_LOG(LogActor, Error, TEXT("AActor::IncrementalRegisterComponents parent component '%s' cannot be registered in actor '%s'"), *GetPathNameSafe(UnregisteredParentComponent), *GetPathName());
					break;
				}

				// Register parent first, then return to this component on a next iteration
				Component = UnregisteredParentComponent;
				CompIdx--;
				NumTotalRegisteredComponents--; // because we will try to register the parent again later...
			}
#if PERF_TRACK_DETAILED_ASYNC_STATS
			FScopeCycleCounterUObject ContextScope(Component);
#endif
				
			// Before we register our component, save it to our transaction buffer so if "undone" it will return to an unregistered state.
			// This should prevent unwanted components hanging around when undoing a copy/paste or duplication action.
			Component->Modify(false);

			Component->RegisterComponentWithWorld(World, Context);
			NumRegisteredComponentsThisRun++;
		}

		NumTotalRegisteredComponents++;
	}

	// See whether we are done
	if (Components.Num() == NumTotalRegisteredComponents)
	{
#if PERF_TRACK_DETAILED_ASYNC_STATS
		QUICK_SCOPE_CYCLE_COUNTER(STAT_AActor_IncrementalRegisterComponents_PostRegisterAllComponents);
#endif
		
		// Skip the world post register if optimizations are enabled and it was already called
		const bool bCallWorldPostRegister = (!bHasRegisteredAllComponents || GOptimizeActorRegistration == 0);

		// Clear this flag as it's no longer deferred
		bHasDeferredComponentRegistration = false;

		bHasRegisteredAllComponents = true;
		// Finally, call PostRegisterAllComponents
		PostRegisterAllComponents();

		if (bCallWorldPostRegister)
		{
			// After all components have been registered the actor is considered fully added: notify the owning world.
			World->NotifyPostRegisterAllActorComponents(this);
		}
		return true;
	}
	
	// Still have components to register
	return false;
}
```