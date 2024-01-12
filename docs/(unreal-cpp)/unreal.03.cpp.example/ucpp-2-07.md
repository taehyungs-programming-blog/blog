---
layout: default
title: "07. Animation Montage"
parent: "(Example2 - 3D 바람의 나라)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_Basic_2/tree/6)

* **Montage** - 이미지 일부를 잘라내 한 화면에서 합성하는 회화 기법
    * UE에서는 애니메이션을 조합하여 재생하려한다

* Animation -> Animation Montage 생성
    * 어렵지않기에 설명은 생략 ...

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_7_1.png"/>
</p>

* Action을 추가하여 Montage를 실행해 보자
    * Input\Actions\IA_Attack
* IMC도 수정해야함을 잊지 말기

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_7_2.png"/>
</p>

```cpp
UCLASS()
class ARENABATTLE_API AABCharacterPlayer : public AABCharacterBase
{
	GENERATED_BODY()
	
public:
	AABCharacterPlayer();

    // ...

    // AttackAction 등록
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

    // ...
```

```cpp
void AABCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// ...

	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);
}

// ...

void AABCharacterPlayer::Attack()
{
	ProcessComboCommand();
}
```

* AABCharacterBase도 수정이 필요하다

```cpp
UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

    // ...

// Combo Action Section
protected:

    // Montage를 입력받음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UABComboActionData> ComboActionData;

	void ProcessComboCommand();

	void ComboActionBegin();
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);
	void SetComboCheckTimer();
	void ComboCheck();

	int32 CurrentCombo = 0;
	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;
};
```

```cpp
void AABCharacterBase::ComboActionBegin()
{
	// Combo Status
	CurrentCombo = 1;

	// Movement Setting
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// Animation Setting
	const float AttackSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    // ComboActionMontage로 Montage를 받는다
        // 근데 여기로 들어오는 Montage는 어디서 설정하나?
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}
```

* 위에서 받아올 Montage는 Blueprint에서 설정하는게 좋다(안그럼 매번 하드코딩 해야함)
    * 아래처럼 BluePrint에서 Montage를 지정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_7_3.png"/>
</p>

* Blueprint Animation에도 Slot을 등록해 Montage를 실행시킬준비를 해야한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_7_4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_7_5.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_7_6.png"/>
</p>

* 게임모드가 잘 적용이 안된다면 Editor를 한 번 껏다가 키자

---

* 콤보 공격 구현

* 우선 Data Asset을 하나 추가하자
    * C++ Data Asset을 추가하자
    * `ABComboActionData`
    * Blueprint도 Content\CharacterAction\ABA_ComboAttack

```cpp
UCLASS()
class ARENABATTLE_API UABComboActionData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UABComboActionData();

	UPROPERTY(EditAnywhere, Category = Name)
	FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category = Name)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category = Name)
	float FrameRate;

	UPROPERTY(EditAnywhere, Category = ComboData)
	TArray<float> EffectiveFrameCount;
};
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_7_7.png"/>
</p>

```cpp
void AABCharacterBase::ProcessComboCommand()
{
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

    // 이전에 실행중인 콤보가 있는지 체크
	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}
}
```

```cpp
void AABCharacterBase::ComboActionBegin()
{
	// Combo Status
        // 1부터 Combo가 시작된다는 뜻
	CurrentCombo = 1;

	// Movement Setting
        // MoveMode를 None으로 설정(움직이면서 스킬쓸수 없게)
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// Animation Setting

    // 애니메이션 실행
	const float AttackSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);

    // 애니메이션 종료를 받는다
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);

    // 콤보가 끝낫기에 0으로
	CurrentCombo = 0;

    // 다시 Waliking이 가능하게 변경
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}
```

```cpp
void AABCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	const float AttackSpeedRate = 1.0f;
    // Combo에 필요한 시간을 체크한다.
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) / AttackSpeedRate;

    // 0보다 작다면 문제 있는 것.
	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void AABCharacterBase::ComboCheck()
{
    // 타이머를 초기화 해주고
	ComboTimerHandle.Invalidate();

    // 첫 번째 콤보가 아니라 다음 콤보부터는 여기를 탄다
	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);

        // 해당 Section으로 점푸!
		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);
		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}
```