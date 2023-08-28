---
layout: default
title: "03. 몬스터 생성"
parent: "(Unreal C++ RPG Action만 구현)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* C++ 클래스로 Creature를 만들어 보자
* 우선 충돌처리를 먼저 넣어보자.

```cpp
UPROPERTY(VisibleAnywhere, Category = "Collision", meta = (AllowPrivateAccess = true))
class USphereComponent* SphereComponent;
```

```cpp
ACreature::ACreature()
{
 	// ...

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	
	SphereComponent->InitSphereRadius(20.0f);
	
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	FName WeaponSocket = FName(TEXT("RightHandSocket"));
	
	SphereComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);

    // ...
```

* 이후 애니메이션을 넣어야 하는데 리타게터를 적당히 잘 넣자 ㅎㅎ (이전에 한거라 생략)
    * 애니메이션 우 클릭 후 애니메이션 애셋 리타겟 -> IK 리타게터 세팅
* 애니메이션 넣기

```cpp
UPROPERTY(EditAnywhere, Category = "Pawn", meta = (AllowPrivateAccess = true))
class UAnimMontage* AttackAnimMontate;
```

```cpp
void ACreature::StartAttack()
{
	if (bIsDuringAttack == false)
	{
		bIsDuringAttack = true;
		
		float AttackAnimDuration = PlayAnimMontage(AttackAnimMontate);
		FTimerHandle TimerHandle;
		
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ACreature::EndAttack, AttackAnimDuration);
	}
}
```

* 충돌체크도 넣자

```cpp
virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
```

* 캐릭터 방향으로 회전하기 만들어 보자

```cpp
virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime) override;

{
    // 코드는 회전값을 보간하는 정도이다
    FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewControlRotation, DeltaTime, 6.0f);
	
	Super::FaceRotation(CurrentRotation, DeltaTime);
}
```

---

* 이제 Creature Blueprint를 만들어 보자
    * 여기도 별거 없기에 생략
* CreatureAnimInstance 클래스 생성, Animation Blueprint의 부모로 선택

```cpp
UCLASS()
class PROJECTRPG_API UCreatureAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pan", meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pan", meta = (AllowPrivateAccess = true))
	bool bIsInAir;

private:

public:
	UFUNCTION()
	void AnimNotify_SetInitCollision();

	UFUNCTION()
	void AnimNotify_SetWeaponCollisionEnabled();

	UFUNCTION()
	void AnimNotify_SetWeaponCollisionDisabled();
	
};
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-3-1.png"/>
</p>

* 애니메이션 blueprint를 지정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-3-2.png"/>
</p>