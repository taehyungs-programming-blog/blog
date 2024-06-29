---
layout: default
title: "07. InitState"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/LyraClone/tree/2)
	* 이거말고 LyraStarterGame을 봐도 됩니다.

---

## IGameFrameworkInitStateInterface란?

* **IGameFrameworkInitStateInterface**
    * Unreal Engine에서 게임 프레임워크 초기화 상태를 관리하는 데 사용되는 인터페이스입니다. 
    * 이 인터페이스는 게임의 초기화 과정에서 다양한 상태 전환을 처리하기 위해 설계되었습니다. 이를 통해 게임 로직이 초기화되는 동안 특정 상태를 설정하고 이를 기반으로 적절한 동작을 수행할 수 있습니다.
* IGameFrameworkInitStateInterface의 주요 기능은 다음과 같습니다:
    * 초기화 상태 관리: 게임이 초기화될 때 여러 가지 상태를 정의하고, 이러한 상태를 관리할 수 있습니다. 이를 통해 게임의 초기화 과정이 체계적으로 이루어질 수 있습니다.
    * 상태 전환: 초기화 상태 간의 전환을 처리할 수 있는 메서드를 제공합니다. 이는 상태 전환이 필요할 때 적절한 동작을 수행하는 데 도움이 됩니다.
    * 상태 확인: 현재 상태를 확인하고 특정 상태에 따른 로직을 구현할 수 있습니다. 이를 통해 특정 상태에서만 실행되어야 하는 코드를 쉽게 관리할 수 있습니다.
    * 게임 프레임워크와의 통합: 게임 프레임워크의 다른 부분과 통합되어 초기화 과정에서 필요한 다양한 기능을 사용할 수 있습니다.

```cpp
UCLASS()
class MYGAME_API AMyGameInitState : public AActor, public IGameFrameworkInitStateInterface
{
    GENERATED_BODY()

public:
    //~ Begin IGameFrameworkInitStateInterface interface
    virtual FName GetFeatureName() const override;
    virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
    virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
    virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
    virtual void CheckDefaultInitialization() override;
    //~ End IGameFrameworkInitStateInterface interface
};
```

```cpp
FName AMyGameInitState::GetFeatureName() const
{
    return FName(TEXT("MyGameFeature"));
}

bool AMyGameInitState::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
    // 상태 변경이 가능한지 여부를 결정하는 로직을 구현
    // 예시: 특정 조건을 만족하는 경우에만 상태 변경을 허용
    if (CurrentState != DesiredState)
    {
        return true;
    }
    return false;
}

void AMyGameInitState::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
    // 상태 변경 시 필요한 로직을 구현
    UE_LOG(LogTemp, Log, TEXT("Changing init state from %s to %s"), *CurrentState.ToString(), *DesiredState.ToString());
    if (DesiredState == FGameplayTag::RequestGameplayTag(FName("Game.Initialized")))
    {
        // 초기화 상태로 변경되는 경우의 로직
        UE_LOG(LogTemp, Log, TEXT("Initialization complete"));
    }
}

void AMyGameInitState::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
    // 액터의 초기화 상태가 변경될 때 실행할 로직
    UE_LOG(LogTemp, Log, TEXT("Actor init state changed to %s"), *Params.NewState.ToString());
}

void AMyGameInitState::CheckDefaultInitialization()
{
    // 기본 초기화 상태를 확인하고 필요한 경우 초기화 로직을 실행
    UE_LOG(LogTemp, Log, TEXT("Checking default initialization"));
    // 기본 초기화 상태로 설정
    FGameplayTag InitialState = FGameplayTag::RequestGameplayTag(FName("Game.Default"));
    HandleChangeInitState(nullptr, FGameplayTag(), InitialState);
}

```

```cpp
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 게임 모드 시작 시 초기화 상태 설정
    AMyGameInitState* InitStateActor = GetWorld()->SpawnActor<AMyGameInitState>();
    if (InitStateActor)
    {
        InitStateActor->CheckDefaultInitialization();
        
        // 초기화 상태 변경 예시
        FGameplayTag CurrentState = FGameplayTag::RequestGameplayTag(FName("Game.Default"));
        FGameplayTag DesiredState = FGameplayTag::RequestGameplayTag(FName("Game.Initialized"));
        
        if (InitStateActor->CanChangeInitState(nullptr, CurrentState, DesiredState))
        {
            InitStateActor->HandleChangeInitState(nullptr, CurrentState, DesiredState);
        }
    }
}
```

---

## Lyra에서는 어떻게 상되는지 보자

* `IGameFrameworkInitStateInterface`를 상속해 사용하는 클래스가 있다. 왜? 사용할까?

```cpp
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class LYRAGAME_API ULyraHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	// ...

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface
```

* 어디서 선언이 될까?

```cpp
void ULyraGameInstance::Init()
{
	Super::Init();

	// Register our custom init states
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(LyraGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(LyraGameplayTags::InitState_DataAvailable, false, LyraGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(LyraGameplayTags::InitState_DataInitialized, false, LyraGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(LyraGameplayTags::InitState_GameplayReady, false, LyraGameplayTags::InitState_DataInitialized);
/*
시작 -> LyraGameplayTags::InitState_Spawned
LyraGameplayTags::InitState_Spawned -> InitState_DataAvailable
InitState_DataAvailable -> InitState_DataInitialized
InitState_DataInitialized -> InitState_GameplayReady

순서라 생각하면되겠다.
각 Component에서 상태를 변경하고 모두 같은 상태가 되면 Manager에서 현재 상태를 변경하는 과정을 겪는다.
*/
	}
```

```cpp
// 다시 여기로 돌아와서

// 상태를 변경할 수 있는지 묻는다
virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;

// 상태를 변경하겠다 알린다
virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;

// 상태 변경이 될 경우 여기로 온다.
virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
```

* 왜 이렇게 할까?
    * 여러 Component를 별도로 관리하며 State를 통일해 주기 위해서이다.

```cpp
// 실제론 이렇게 변경
void ULyraHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(ULyraPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
    // 여기서 State를 변경요청
	ensure(TryToChangeInitState(LyraGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}
```

