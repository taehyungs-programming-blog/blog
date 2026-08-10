---
layout: default
title: "04. 몬스터 AI"
parent: "(Example - RPG Action)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 우선 AIController를 하나 만들자
* C++코드에서 AIController를 사용하기 위해서 모듈추가가 필요하다

```csharp
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectRPG : ModuleRules
{
	public ProjectRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", 
        // 요렇게 세 개 필요함
        "AIModule", "GameplayTasks", "NavigationSystem", 
        "UMG" });
	}
}

```

```cpp
UCLASS()
class PROJECTRPG_API ACreatureAIController : public AAIController
{
	GENERATED_BODY()

 public:
	 ACreatureAIController();

	 virtual void OnPossess(APawn* InPawn) override;

	 virtual void OnUnPossess() override;

	 virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;

private:
	UPROPERTY(Transient, meta = (AllowPrivateAccess = true))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(Transient, meta = (AllowPrivateAccess = true))
	class UBlackboardComponent* BlackboardComponent;

	int32 TargetID;
	
};
```

```cpp
ACreatureAIController::ACreatureAIController()
{
    // 생성
	BehaviorTreeComponent =
		CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	BlackboardComponent =
		CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}
```

* Creature에서 Controller를 지정하자

```cpp
AIControllerClass = ACreatureAIController::StaticClass();

AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
```

* Blackboard, Behavior Tree를 만들어 주자
* Blackboard에 사용될 변수를 선언하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-4-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-4-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-4-3.png"/>
</p>

* 길찾기를 넣으려면 NavMeshBoundsVolume이 필요하다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-4-4.png"/>
</p>

* 적당히 구역을 잡고 show -> Navigation으로 확인해 봐야한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-4-5.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-4-6.png"/>
</p>

* 서비스도 하나 생성

```cpp
UCLASS()
class PROJECTRPG_API UBTService_SearchTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SearchTarget();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
```

```cpp
void UBTService_SearchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	APawn* CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	if (CurrentPawn == nullptr)
	{
		return;
	}
	
	UWorld* World = CurrentPawn->GetWorld();
	
	FVector Center = CurrentPawn->GetActorLocation();

	
	float SearchRadius = 500.0f;

	if (World == nullptr)
	{
		return;
	}
	
	TArray<FOverlapResult> OverlapResults;
	
	FCollisionQueryParams QueryParams(NAME_None, false, CurrentPawn);
	
	bool bResult = World->OverlapMultiByChannel
	(
		OverlapResults, Center, FQuat::Identity,
		
		ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeSphere(SearchRadius), QueryParams
	);
	
	if (bResult)
	{
		for (auto& OverlapResult : OverlapResults)
		{
			AProjectRPGCharacter* Target = Cast<AProjectRPGCharacter>(OverlapResult.GetActor());

			if (Target && Target->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName(TEXT("Targetkey")), Target);
				return;
			}
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName(TEXT("Targetkey")), nullptr);
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName(TEXT("Targetkey")), nullptr);
	}
}
```

---

* 여기서 부턴 새로 정의한 Behavior Tree 코드라 필요하다면 찾아서 보자

```cpp
UCLASS()
class PROJECTRPG_API UBTTask_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TurnToTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
```

```cpp
UCLASS()
class PROJECTRPG_API UBTTask_Patrol : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
```

```cpp
UCLASS()
class PROJECTRPG_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_Attack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bIsAttacking = false;
	
};
```

```cpp
UCLASS()
class PROJECTRPG_API UBTService_SearchTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SearchTarget();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
```

```cpp
UCLASS()
class PROJECTRPG_API UBTDecorator_CanAtrtack : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CanAtrtack();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
```