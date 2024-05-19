---
layout: default
title: "24. animation event"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/2)

---

* C++ 클래스에서 AnimNotify를 하나 만든다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-24-1.png"/>
</p>

* 몽타주에서 등록한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-24-2.png"/>
</p>

```cpp
UCLASS()
class R1_API UR1AnimNotify_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UR1AnimNotify_SendGameplayEvent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;
};
```

```cpp
void UR1AnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AR1Character* LocalCharacter = Cast<AR1Character>(MeshComp->GetOwner());
	if (LocalCharacter)
	{
		LocalCharacter->HandleGameplayEvent(EventTag);
	}
}
```

```cpp
void AR1Player::HandleGameplayEvent(FGameplayTag EventTag)
{
	AR1PlayerController* PC = Cast<AR1PlayerController>(GetController());
	if (PC)
	{
		PC->HandleGameplayEvent(EventTag);
	}
}
```

```cpp
void AR1PlayerController::HandleGameplayEvent(FGameplayTag EventTag)
{
	if (EventTag.MatchesTag(R1GameplayTags::Event_Montage_Attack))
	{
		if (TargetActor)
		{
			TargetActor->OnDamaged(R1Player->FinalDamage, R1Player);
		}
	}
}
```

```cpp
void AR1Character::OnDamaged(int32 Damage, TObjectPtr<AR1Character> Attacker)
{
	Hp = FMath::Clamp(Hp - Damage, 0, MaxHp);
	if (Hp == 0)
	{
		OnDead(Attacker);
	}

	D(FString::Printf(TEXT("%d"), Hp));
}
```