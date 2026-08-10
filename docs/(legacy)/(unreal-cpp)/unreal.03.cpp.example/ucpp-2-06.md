---
layout: default
title: "06. Character Animation"
parent: "(Example2 - 3D 바람의 나라)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_Basic_2/tree/5)

## Market Place에서 Animation을 다운받아보자

* Infinity Blade: Warriors사용예정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_1.png"/>
</p>

* 다운 후 프로젝트에 추가할 것

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_2.png"/>
</p>

* Animation 추가
    * `D:\...\Content\ArenaBattle\Animation` 아래 이미 넣어둠. (코드 참조)

---

## Animation을 담당하는 C++ 클래스를 생성해 보자

* Tools -> Create New C++ Class -> AnimInstance 생성
    * 이름 `ABAnimInstance`
* 그리고 Content/Animation 폴더에 Animation BluePrint도 하나 생성하자
    * Animation -> Animation Blueprint 생성
    * SK_Mannequin_Skeleton을 선택하고 Parent를 `ABAnimInstance`로 지정한다
    * 이름 `ABP_ABCharacter`
* 캐릭터를 이제 Infinity Blade: Warriors에서 로딩된 캐릭터로 수정하자

```cpp
AABCharacterBase::AABCharacterBase()
{
	// ...

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

    // Infinity Blade: Warriors의 SK_CharM_Cardboard로 기본 Mesh를 수정함
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

    // 바꾸는 김에 AnimInstance도 바꿔보자
    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

    // ...
```

* 여기까지하면 캐릭터는 변경되지만 애니메이션이 적용이 안됨

---

## 애니메이션을 적용해 보자

```cpp
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();

protected:
    // 말 그대로 Init
	virtual void NativeInitializeAnimation() override;

    // 말 그대로 Update
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshould;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshould;
};
```

```cpp
UABAnimInstance::UABAnimInstance()
{
	MovingThreshould = 3.0f;
	JumpingThreshould = 100.0f;
}

void UABAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
	}
}
```

* 자! 이제 Blue print 차례이다.

* `ABP_ABCharacter`를 보면 C++에서 생성한 변수가 안보이는데
* 상속받은 변수를 보기 위해선 설정 -> Show Inherited Variables를 체크해야한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_4.png"/>
</p>

* AnimGraph에서 우클릭 후 Statemachine 추가
* Statemachine에서
    * Add State
    * 단, State의 상태가 매우 복잡할 경우 별도로 State를 뺄수 있다.(Cache로 저장하는 방식)
    * 다시 AnimGraph에서 Statemachine추가

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_5.png"/>
</p>

* 요런식. 위의 Locomotion에서 생성된 값을 Cache에 저장후 Output Pose에서 사용하게 한다. 라고 정도 알아두자
* 여기서 부턴 모두 설명하진 못하고 어렵다 싶은 부분만 설명할 예정, 코드를 보자

* 우선! 달리는 모션에 따라 Idle <-> Walk <-> Run 을 Blend해줘야 한다.
    * BluePrint에 Animation -> Blueprint -> Legacy -> Blend 1D를 추가
    * 가로축의 이름을 GroundSpeed로 바꾸고 최대값을 500으로 설정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_6.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_7.png"/>
</p>

* 이제 Blueaprint에서 생성한 Blend를 사용해 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_8.png"/>
</p>

* 이제 Locomotion을 Cache로 저장하게 해보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_9.png"/>
</p>

---

## 추가

* 약간 설명이 부족한거 같아 설명을 추가한다.
* 우선 Blueprint Anim인 ABP_ABCharacter의 AnimGraph에 들어가보면

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_10.png"/>
</p>

* State Machine이 두 개(Locomotion, MainStateMachine) 존재한다.
* 우선 Locomotion의 output이 LocomotionCache에 캐쉬되며 MainStateMachine의 결과가 Output으로 나감을 확인할 수 있다.
* Locomotion State부터 보자면

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_11.png"/>
</p>

* Idle 혹은 Walk상태를 분석 후 Output(cache)로 내보낸다.
* 다음은 MainStateMachine을 보자면

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_6_12.png"/>
</p>

* 현재 상태가 Jump나 Land가 아닐경우 Locomotion(Cache된 값)으로 Output을 내보내고 Jump, Land일 경우 다른 애니메이션을 내보낸다
