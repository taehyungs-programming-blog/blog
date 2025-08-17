---
layout: default
title: "04. Actor는 어떻게 등록 될까?"
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
UWorld* UWorld::CreateWorld(
    const EWorldType::Type InWorldType, 
    bool bInformEngineOfWorld, 
    FName WorldName, 
    UPackage* InWorldPackage, 
    bool bAddToRoot, 
    ERHIFeatureLevel::Type InFeatureLevel, 
    const InitializationValues* InIVS, 
    bool bInSkipInitWorld)
{
    // ...

    // CreateWorld와 동시에 InitializeNewWorld 가 호출된다.
    UPackage* WorldPackage = InWorldPackage;
	if ( !WorldPackage )
	{
		WorldPackage = CreatePackage(nullptr);
	}

	UWorld* NewWorld = NewObject<UWorld>(WorldPackage, *WorldNameString);
	
	NewWorld->InitializeNewWorld(InIVS ? *InIVS : UWorld::InitializationValues()
		.CreatePhysicsScene(InWorldType != EWorldType::Inactive)
		.ShouldSimulatePhysics(false)
		.EnableTraceCollision(true)
		.CreateNavigation(InWorldType == EWorldType::Editor)
		.CreateAISystem(InWorldType == EWorldType::Editor)
		,bInSkipInitWorld);

    // ...
```

```cpp
void UWorld::InitializeNewWorld(
    const InitializationValues IVS, 
    bool bInSkipInitWorld)
{
    // ...

	InitWorld(IVS);

    // InitWorld 후 UpdateWorldComponents 호출
    UpdateWorldComponents(bRerunConstructionScripts, false);
```

```cpp
void UWorld::UpdateWorldComponents(
    bool bRerunConstructionScripts, 
    bool bCurrentLevelOnly, 
    FRegisterComponentContext* Context)
{
    // ...

	for( int32 LevelIndex=0; LevelIndex<Levels.Num(); LevelIndex++ )
	{
        // ...

	    if(!StreamingLevel || Level->bIsVisible)
		{
		    // 여기서 Level내의 Component를 업데이트
			Level->UpdateLevelComponents(bRerunConstructionScripts, Context);
			IStreamingManager::Get().AddLevel(Level);

    // ...
```

> 여기 중간과정은 생략

```cpp
void ULevel::IncrementalUpdateComponents(int32 NumComponentsToUpdate, bool bRerunConstructionScripts, FRegisterComponentContext* Context)
{
	// NumComponentsToUpdate가 0일 경우 모두 업데이트
    bool bFullyUpdateComponents = (NumComponentsToUpdate == 0);

    // ...

    do
	{
	    switch (IncrementalComponentState)
		{
		   	case EIncrementalComponentState::Init:
		   		SortActorsHierarchy(Actors, this);
			    IncrementalComponentState = EIncrementalComponentState::RegisterInitialComponents;

			case EIncrementalComponentState::RegisterInitialComponents:
		    	if (IncrementalRegisterComponents(true, NumComponentsToUpdate, Context))
			    {
				    IncrementalComponentState = EIncrementalComponentState::Finalize;
			    }
			    break;

		    case EIncrementalComponentState::Finalize:
		    	IncrementalComponentState = EIncrementalComponentState::Init;
			    CurrentActorIndexForIncrementalUpdate = 0;
			    bHasCurrentActorCalledPreRegister = false;
			    bAreComponentsCurrentlyRegistered = true;
			    CreateCluster();
			    break;
		}
	} while(bFullyUpdateComponents && !bAreComponentsCurrentlyRegistered);

	FPhysScene* PhysScene = OwningWorld->GetPhysicsScene();
	if (PhysScene)
	{
		PhysScene->ProcessDeferredCreatePhysicsState();
	}
}
```

### 마지막 ProcessDeferredCreatePhysicsState 처리 이유?

* `ULevel::IncrementalUpdateComponents`의 마지막에 `PhysScene->ProcessDeferredCreatePhysicsState();`를 호출하는 이유는 컴포넌트 등록 과정에서 물리 상태(Physics State) 생성이 지연(deferred)되었기 때문

* IncrementalUpdateComponents는 레벨에 있는 액터들의 컴포넌트들을 순차적으로 등록하는 과정
* 이 과정에서 UPrimitiveComponent 같은 물리 기반 컴포넌트들은 RegisterComponent 중에 **즉시 물리 엔진에 등록되는 게 아니라** "나중에 처리해야 할 물리 상태 생성 요청"을 **쌓는다**
* 즉, 물리 월드에 곧바로 반영하지 않고, 지연 등록(Deferred) 방식으로 처리

### ProcessDeferredCreatePhysicsState()의 역할

* 이 함수는 PhysX/Chaos(언리얼의 물리 엔진)에 모든 지연된 물리 상태 생성 요청을 실제로 처리
    * 콜리전(충돌) shape 등록
    * 물리 본체(Body Instance) 생성
    * 시뮬레이션 월드에 추가
    * 이 시점에서야 비로소 게임 월드에 물리적으로 존재할 수 있게 됩니다.

### 띄용? 언제 Physics에 Component를 등록했지?

* Level::IncrementalUpdateComponents() → 액터/컴포넌트 등록 과정에서 UPrimitiveComponent::OnRegister()/RecreatePhysicsState()가 호출
* 이때 물리 상태를 “즉시” 만들지 않고, 물리 생성 요청을 FPhysScene(Chaos 씬)의 대기열에 넣습니다.

```cpp
// 대략 이런 형태이다.
void UPrimitiveComponent::OnRegister()
{
    if (PhysScene->IsDeferringCreateDestroy()) {
        PhysScene->AddPendingOnCreatePhysicsState(this); // 약한 참조(Weak)로 보관
    } else {
        CreatePhysicsState(); // 바로 생성 (지연이 필요 없을 때)
    }
}
```