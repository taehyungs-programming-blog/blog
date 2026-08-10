---
layout: default
title: "06. Component 등록한다의 개념 및 코드"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Component를 등록한다란?

* World에 컴포넌트를 등록한다는 개념은, 액터가 소유한 컴포넌트들을 게임의 다양한 "월드" (게임 스레드, 물리 스레드, 렌더 스레드)에 맞게 적절히 전달하여 해당 월드에서 컴포넌트의 기능이 정상적으로 동작하도록 하는 것을 의미

* 각 월드별로 컴포넌트 등록의 의미를 좀 더 자세히 설명하면 다음과 같다
    * Game World (게임 스레드): 액터 컴포넌트의 게임 로직과 관련된 부분이 게임 스레드에 등록. 예를 들어, 캐릭터의 움직임을 담당하는 CharacterMovementComponent나 아이템의 상호작용을 처리하는 InteractionComponent 등은 게임 스레드에서 업데이트되고 처리
    * Physics World (물리 스레드/서브스텝): StaticMeshComponent나 SkeletalMeshComponent와 같이 물리적 속성을 가지는 컴포넌트들이 물리 월드에 등록. 이를 통해 충돌 감지, 물리 시뮬레이션 등이 물리 스레드에서 효율적으로 처리. 언리얼 엔진에서는 물리 시뮬레이션이 게임 스레드와 별개의 물리 스레드에서 서브스텝으로 동작하여 게임의 프레임 속도에 영향을 주지 않으면서 정교한 물리 연산을 수행
    * Render World (렌더 스레드): 화면에 보여지는 MeshComponent나 ParticleSystemComponent와 같은 렌더링 관련 컴포넌트들이 렌더 스레드에 등록됩니다. 렌더 스레드는 이러한 컴포넌트들의 시각적 정보를 처리하여 최종적으로 화면에 이미지를 그려내는 역할.
* 결론적으로 "컴포넌트를 등록한다"는 것은 각 컴포넌트의 역할과 특성에 맞춰 언리얼 엔진의 내부 시스템(게임 스레드, 물리 스레드, 렌더 스레드)에 해당 컴포넌트를 연결하여 게임 내에서 올바르게 작동하도록 하는 과정을 의미

---

## 그럼 이제 코드로 보자

```cpp
void UActorComponent::RegisterComponentWithWorld(UWorld* InWorld, FRegisterComponentContext* Context)
{
	// ...

	// 등록 여부 체크
	if(IsRegistered())
	{
		UE_LOG(LogActorComponent, Log, TEXT("RegisterComponentWithWorld: (%s) Already registered. Aborting."), *GetPathName());
		return;
	}

	if(InWorld == nullptr)
	{
		//UE_LOG(LogActorComponent, Log, TEXT("RegisterComponentWithWorld: (%s) NULL InWorld specified. Aborting."), *GetPathName());
		return;
	}

	// If not registered, should not have a scene
	checkf(WorldPrivate == nullptr, TEXT("%s"), *GetFullName());

	AActor* MyOwner = GetOwner();
	checkSlow(MyOwner == nullptr || MyOwner->OwnsComponent(this));

	if (MyOwner && MyOwner->GetClass()->HasAnyClassFlags(CLASS_NewerVersionExists))
	{
		UE_LOG(LogActorComponent, Log, TEXT("RegisterComponentWithWorld: Owner belongs to a DEADCLASS"));
		return;
	}

    // ...

	if (!bHasBeenCreated)
	{
		OnComponentCreated();
	}

	WorldPrivate = InWorld;

    // 결국 여기서 실행된다.
	ExecuteRegisterEvents(Context);

	// If not in a game world register ticks now, otherwise defer until BeginPlay. If no owner we won't trigger BeginPlay either so register now in that case as well.
	if (!InWorld->IsGameWorld())
	{
		RegisterAllComponentTickFunctions(true);
	}
	else if (MyOwner == nullptr)
	{
		if (!bHasBeenInitialized && bWantsInitializeComponent)
		{
			InitializeComponent();
		}

		RegisterAllComponentTickFunctions(true);
	}
	else
	{
        // 여기서 GameWorld에 등록
		MyOwner->HandleRegisterComponentWithWorld(this);
	}

	// ...
}
```

```cpp
void UActorComponent::ExecuteRegisterEvents(FRegisterComponentContext* Context)
{
	if(!bRegistered)
	{
		SCOPE_CYCLE_COUNTER(STAT_ComponentOnRegister);
		OnRegister();
		checkf(bRegistered, TEXT("Failed to route OnRegister (%s)"), *GetFullName());
	}

	if(FApp::CanEverRender() && !bRenderStateCreated && WorldPrivate->Scene && ShouldCreateRenderState())
	{
		SCOPE_CYCLE_COUNTER(STAT_ComponentCreateRenderState);
		LLM_SCOPE(ELLMTag::SceneRender);
		LLM_SCOPE_DYNAMIC_STAT_OBJECTPATH(GetPackage(), ELLMTagSet::Assets);
		UE_TRACE_METADATA_SCOPE_ASSET_FNAME(NAME_None, NAME_None, GetPackage()->GetFName());

        // Render에 Component를 넣고
		CreateRenderState_Concurrent(Context);
		checkf(bRenderStateCreated, TEXT("Failed to route CreateRenderState_Concurrent (%s)"), *GetFullName());
	}

    // 역시 Physics에도 넣는다
	CreatePhysicsState(/*bAllowDeferral=*/true);
}
```