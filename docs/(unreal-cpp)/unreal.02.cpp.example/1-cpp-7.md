---
layout: default
title: "07. 몬스터 동적 할당 + UI 추가"
parent: "(Example - RPG Action)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 랜덤으로 몬스터를 할당할 Blueprint를 하나 만들어보자

```cpp
UCLASS()
class PROJECTRPG_API ARandomPosition : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARandomPosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
```

* ARandomPosition를 기반으로 blueprint생성
    * BP_RandomPosition이라는 이름으로 생성
    * Level에 30개 정도 넣어두자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-7-1.png"/>
</p>

```cpp
void AProjectRPGGameMode::SpawnCreature()
{
	TArray<AActor*> CollectActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARandomPosition::StaticClass(), CollectActors);

	TArray<AActor*> CollectMonsterSpawnPositions;
	
	for (int32 i = 0; i < MaxCountForSpawnedCreature; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, CollectActors.Num() - 1);
		
		CollectMonsterSpawnPositions.AddUnique(CollectActors[RandomIndex]);
		
		CollectActors.RemoveAt(RandomIndex);
	}

	for (int32 i = 0; i < CollectMonsterSpawnPositions.Num(); i++)
	{
		if (CreatureBP)
		{
			FVector SpawnLocation = CollectMonsterSpawnPositions[i]->GetActorLocation();
			
			FRotator SpawnRotation = CollectMonsterSpawnPositions[i]->GetActorRotation();
			
			FActorSpawnParameters SpawnParams;
			
			ACreature* Creature =
				GetWorld()->SpawnActor<ACreature>(CreatureBP, SpawnLocation, SpawnRotation, SpawnParams);
			
			Creatures.AddUnique(Creature);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("CollectActors : %d, Creatures : %d"), CollectActors.Num(), Creatures.Num());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, 
			FString::Printf(TEXT("%d / %d"), Creatures.Num(), MaxCountForSpawnedCreature));
	}
}
```

---

* 체력과 몬스터 상태 UI를 넣어보자

```cpp
UCLASS()
class PROJECTRPG_API UUpperMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* ProgressBarPlayerHealth;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TextPlayerHealth;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TextCreatureCount;	
};
```


