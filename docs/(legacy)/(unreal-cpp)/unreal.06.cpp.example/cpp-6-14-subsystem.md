---
layout: default
title: "14. (Lyra) SubSystem"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 여기서 SubSystem의 모든내용을 다룰순 없고, Lyra에서 사용된 SubSystem중 하나를 살펴보자

---

## (이론) SubSystem

* Unreal Subsystem은 Unreal Engine의 모듈화된 시스템으로, 특정 기능이나 서비스의 관리를 목적으로 설계된 객체입니다. 
* Subsystem은 게임 전반에 걸쳐 공유되는 특정 기능을 제공하며, 전역적인 접근을 가능하게 합니다. 이러한 Subsystem은 다양한 기능을 중심으로 구성될 수 있으며, 각기 다른 레벨에서 동작합니다.
* 종류:
  * Engine Subsystem (UEngineSubsystem): 엔진 레벨에서 동작하는 Subsystem으로, 게임 엔진 전체에서 사용할 수 있는 기능을 제공합니다. 예를 들어, 네트워크 매니저나 글로벌 설정 관리를 포함할 수 있습니다.
  * Editor Subsystem (UEditorSubsystem): 에디터 레벨에서 동작하는 Subsystem으로, Unreal Editor에서만 사용할 수 있는 기능을 제공합니다. 예를 들어, 에디터 내의 특정 툴이나 기능의 관리에 사용됩니다.
  * Game Instance Subsystem (UGameInstanceSubsystem): 게임 인스턴스 레벨에서 동작하는 Subsystem으로, 특정 게임 세션에 종속된 기능을 제공합니다. 예를 들어, 플레이어 데이터 관리나 게임 상태를 포함할 수 있습니다.
  * Local Player Subsystem (ULocalPlayerSubsystem): 로컬 플레이어 레벨에서 동작하는 Subsystem으로, 특정 플레이어에 종속된 기능을 제공합니다. 예를 들어, 로컬 플레이어의 설정이나 입력 처리를 관리할 수 있습니다.
  * World Subsystem (UWorldSubsystem): 월드 레벨에서 동작하는 Subsystem으로, 특정 월드에 종속된 기능을 제공합니다. 예를 들어, 월드의 시간 관리나 환경 설정을 포함할 수 있습니다.
* 특징:
  * Modularity (모듈성): Subsystem은 특정 기능을 모듈화하여, 다른 시스템과 독립적으로 동작할 수 있습니다.
  * Global Access (전역 접근성): Subsystem은 전역적으로 접근할 수 있어, 게임의 다른 부분에서 쉽게 사용할 수 있습니다. (이 특징으로 Singletone을 대신한다)
  * Lifecycle Management (생명주기 관리): Unreal Engine은 Subsystem의 생명주기를 관리하여, 적절한 시점에 초기화하고 종료할 수 있습니다. (이 특징으로 Singletone을 대신한다)
  * Extendability (확장성): 개발자는 자신만의 Subsystem을 만들어 필요에 따라 기능을 확장할 수 있습니다.

---

## Lyra에서의 Subsystem

* 이전 Experience를 학습하며 FeatureAction에 등록시 자동으로 Activate되었던 기억이 있다.
* `UGameFeaturesSubsystem::OnGameFeatureActivating`여기서 부터 시작해 보자

```cpp
void UGameFeaturesSubsystem::OnGameFeatureActivating(const UGameFeatureData* GameFeatureData, const FString& PluginName, FGameFeatureActivatingContext& Context, const FGameFeaturePluginIdentifier& PluginIdentifier)
{
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(GFP_OnActivating_CallbackObservers);
		CallbackObservers(EObserverCallback::Activating, PluginIdentifier, &PluginName, GameFeatureData);
	}

	{
		TRACE_CPUPROFILER_EVENT_SCOPE(GFP_OnActivating_CallbackActions);
		for (UGameFeatureAction* Action : GameFeatureData->GetActions())
		{
			if (Action != nullptr)
			{
				Action->OnGameFeatureActivating(Context);
			}
		}
	}
}
```

* 우리는 GameFeatureAction을 이렇게 등록

```cpp
UCLASS(Abstract)
class UGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

private:
	void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);

	/** Override with the action-specific logic */
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld,);

private:
	TMap<FGameFeatureStateChangeContext, FDelegateHandle> GameInstanceStartHandles;
};
```

* `UGameFeatureAction_WorldActionBase`를 상속한다

```cpp
UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Mapping"))
class UGameFeatureAction_AddInputContextMapping final : public UGameFeatureAction_WorldActionBase
```

* 그럼 `UGameFeatureAction_WorldActionBase`는 어디서 읽어갈까?
* 다시 돌아가 `UGameFeaturesSubsystem::OnGameFeatureActivating` 되며 읽어간다

```cpp
void UGameFeaturesSubsystem::OnGameFeatureActivating(const UGameFeatureData* GameFeatureData, const FString& PluginName, FGameFeatureActivatingContext& Context, const FGameFeaturePluginIdentifier& PluginIdentifier)
{
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(GFP_OnActivating_CallbackObservers);
		CallbackObservers(EObserverCallback::Activating, PluginIdentifier, &PluginName, GameFeatureData);
	}

	{
		TRACE_CPUPROFILER_EVENT_SCOPE(GFP_OnActivating_CallbackActions);
		for (UGameFeatureAction* Action : GameFeatureData->GetActions())
		{
			if (Action != nullptr)
			{
				Action->OnGameFeatureActivating(Context);
			}
		}
	}
}
```

* 그럼 `OnGameFeatureActivating`는 누가 불러주는디?
    * 이 자체는 Engine에서 관리하는 부분임.