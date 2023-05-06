---
layout: default
title: "08. 캐릭터 공격 판정"
parent: "(Unreal C++ Part2 - 게임만들어 보기)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_Basic_2/tree/7)

* 우선 Project Settings에서 Collision 세팅이 필요하다
* Trace Channels -> New Trace Channel -> ABAction 생성
* Preset도 생성이 필요함 -> New -> AbCapsule
    * CollisionEnabled: QueryOnly
    * Object Type: Pawn

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_8_1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_8_2.png"/>
</p>

* Montage를 편집해 공격판정을 넣어보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_8_3.png"/>
</p>

* 공격판정을 넣기위해선 Notify를 생성해야한다.
* Tools -> New C++ Class -> AnimNotify
* ABAnimNotifyState_AttackHit 생성

* Anim C++ 클래스를 추가하면 Montage에서 Notify를 넣을수 있게된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_8_4.png"/>
</p>

* 이제 C++ Notify를 수정해 보자.

```cpp
UCLASS()
class ARENABATTLE_API UAnimNotify_AttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
```

```cpp
void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
        // IABAnimationAttackInterface는 별도로 생성한 인터페이스이다.
            // USkeletalMeshComponent를 매번 별도의 Character 캐스팅하지 않기위해 인터페이스를 만들었다
		IABAnimationAttackInterface* AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner());
		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
```

```cpp
// 이제 캐릭터는 IABAnimationAttackInterface를 참조한다
UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABAnimationAttackInterface
{
    // ...
```

```cpp
UINTERFACE(MinimalAPI)
class UABAnimationAttackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ARENABATTLE_API IABAnimationAttackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AttackHitCheck() = 0;
};
```

* 이제 진짜 히트판정!
* `AttackPawn->AttackHitCheck();`

* 우선 알아둘 점은 월드 트레이싱 함수의 규칙이 존재한다.
* {카테고리1}{카테고리2}{카테고리3}
    * 카테고리1 - 처리 방법
    * 카테고리2 - 대상(Test(테스트용), Single(대상이하나), Multi(여럿))
    * 카테고리3 - 처리 설정(ByChannel(채널 정보를 이용 감지), ByObject(물체 지정된 물리 타입 정보로 감지), ByProfile(프로필 정보로 감지))
* 우리가 쓸 것은 - **{Sweep}{Single}{ByChannel}**

```cpp
// ABCollision.h
    // 코딩의 편리성을 위해 전처리기를 하나 만들어두자

#pragma once

#include "CoreMinimal.h"

#define CPROFILE_ABCAPSULE TEXT("ABCapsule")
#define CPROFILE_ABTRIGGER TEXT("ABTrigger")
#define CCHANNEL_ABACTION ECC_GameTraceChannel1
```

```cpp
void AABCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = 40.0f;
	const float AttackRadius = 50.0f;
	const float AttackDamage = 30.0f;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

    // SweepSingleByChannel는 여기서 사용됨
	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

// 감지된 영역을 표시
#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);

#endif
}
```

* 데미지를 입힐 NPC 캐릭터를 만들어보자.
* Tools -> New C++ Class -> ABCharacterBase 상속

```cpp
UCLASS()
class ARENABATTLE_API AABCharacterNonPlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AABCharacterNonPlayer();

protected:
	void SetDead() override;
	
};
```

```cpp
void AABCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false);
}
```

---

* 죽는 Montage만들기
* 다른건 크게 특별할 것 없고 Enable Auto Blend Out만 False로 둬서 다음동작이 없도록 만들어 주자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_8_5.png"/>
</p>

* C++에서 죽는동작을 넣자

```cpp
void AABCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void AABCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);

    /*
static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
if (DeadMontageRef.Object)
{
    DeadMontage = DeadMontageRef.Object;
}
    */
}
```

