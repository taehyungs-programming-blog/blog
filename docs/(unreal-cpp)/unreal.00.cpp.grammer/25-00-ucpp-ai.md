---
layout: default
title: "25. ai"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/3)

---

## AIController를 만들기

* C++ 클래스에서 AIController를 만들어보자.

```cpp
UCLASS()
class R1_API AR1AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AR1AIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnPossess(class APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
```

* Monster의 Pawn을 AIController로 세팅하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-25-1.png"/>
</p>

* 몬스터가 갈 수 있는 지역을 세팅하기위해 NavMeshBoundsVolume을 Level에 붙인다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-25-2.png"/>
</p>

---

## Behavior Tree & Blackborad

* 아래처럼 생각하면 편하다.
* Behavior Tree - 의사결정
* Blackborad - 상태 저장

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-25-3.png"/>
</p>

* Behavior Tree & Blackborad의 시작은 Controller에서 하게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-25-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-25-5.png"/>
</p>

* Selector 자식 노드를 왼쪽에서 부터 오른쪽으로 실행
    * 자식 노드가 성공시 다시 처음부터
    * Cf. Sequence 자식 노드가 실패 시 다시 처음 부터

---

## 이걸 C++로 만들어 보자

* 상세한 내용은 코드 참조.

```cpp
UCLASS()
class R1_API UBTTaskNode_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector PatrolPosKey;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	float SearchRadius = 1000.f;
};
```
