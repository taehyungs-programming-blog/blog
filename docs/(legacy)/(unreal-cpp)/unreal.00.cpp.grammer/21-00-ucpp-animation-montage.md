---
layout: default
title: "21. 애니메이션 몽타주"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/2)

---

* 여러 애니메이션을 조합하거나 Noti(Event)를 붙이거나 등을 할 수 있음.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-21-1.png"/>
</p>

* 여기서 한 가지 궁금증이 해결된다.
* Animation BP에서 Default Slot이 뭔가 했는데

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-21-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-21-3.png"/>
</p>

* 몽타주를 디폴트 슬롯에 넣는다는 것이다.
* 몽타주는 PlayerController에 담아둔다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-21-4.png"/>
</p>

```cpp
void AR1PlayerController::ChaseTargetAndAttack()
{
	if (TargetActor == nullptr)
	{
		return;
	}

	if (GetCreatureState() == ECreatureState::Skill)
	{
		return;
	}

	FVector Direction = TargetActor->GetActorLocation() - R1Player->GetActorLocation();
	if (Direction.Length() < 250.f)
	{
		GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::Cyan, TEXT("Attack"));

		if (AttackMontage)
		{
			if (bMousePressed)
			{
				FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(R1Player->GetActorLocation(), TargetActor->GetActorLocation());
				R1Player->SetActorRotation(Rotator);

				GetCharacter()->PlayAnimMontage(AttackMontage);
				SetCreatureState(ECreatureState::Skill);

				TargetActor = HighlightActor;
			}
			else
			{
				TargetActor = nullptr;
			}
		}
	}
	else
	{
		FVector WorldDirection = Direction.GetSafeNormal();
		R1Player->AddMovementInput(WorldDirection, 1.0, false);
	}
}
```