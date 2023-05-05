---
layout: default
title: "05. Data Asset"
parent: "(Unreal C++ Part2 - 게임만들어 보기)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_Basic_2/tree/4)

* 앞서 아주 많은 Option이 있음을 확인했다
* 이걸 매번 외워서 쓰기는 힘들다

## Data Asset을 만들어 분산된 Option을 한번에 관리해보자.

* Tools -> New C++ Class -> PrimaryDataAsset
    * `D:/Git/Unreal_Cpp_Basic_2/Source/ArenaBattle/Character/`
    * ABCharacterControlData로 이름 지정

```cpp
// 사용할 Option 변수를 지정해 보자

UCLASS()
class ARENABATTLE_API UABCharacterControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	public:
	UABCharacterControlData();

	UPROPERTY(EditAnywhere, Category = Pawn)
	uint32 bUseControllerRotationYaw : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint32 bOrientRotationToMovement : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	uint32 bUseControllerDesiredRotation : 1;

	UPROPERTY(EditAnywhere, Category = CharacterMovement)
	FRotator RotationRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	float TargetArmLength;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	FRotator RelativeRotation;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bUsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bInheritPitch : 1;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bInheritYaw : 1;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bInheritRoll : 1;

	UPROPERTY(EditAnywhere, Category = SpringArm)
	uint32 bDoCollisionTest : 1;

};
```

* 이제 사용해보자.
* Content\ArenaBattle\CharacterControl아래
* Add -> Miscellaneous -> Data Asset -> ABCharacterControlData

* 두 가지를 만들것이다.
    * Quater View
    * Shoulder View

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_5_1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_5_2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_5_3.png"/>
</p>

* v를 통해 뷰를 바꿀 수 있게 해보자

```cpp
// Enum으로 Shoulder, Quater를 관리
UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quater
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
    // Shoulder, Quater를 설정하는 부분
	virtual void SetCharacterControlData(const class UABCharacterControlData* CharacterControlData);

    // Map으로 관리한다.
	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

};
```

```cpp
AABCharacterBase::AABCharacterBase()
{
	// ...

    // 처음에 Map에 데이터를 넣어두자
	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (ShoulderDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuaterDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'"));
	if (QuaterDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Quater, QuaterDataRef.Object);
	}
}
```

* 이제 사용될 Player 클래스
* 6개의 함수가 추가된다.
    * `void ChangeCharacterControl();`
	* `void SetCharacterControl(ECharacterControlType NewCharacterControlType);`
	* `virtual void SetCharacterControlData(const class UABCharacterControlData* CharacterControlData) override;`
    * `void ShoulderMove(const FInputActionValue& Value);`
	* `void ShoulderLook(const FInputActionValue& Value);`
	* `void QuaterMove(const FInputActionValue& Value);`

* 첫 번째로 볼 함수는 `SetCharacterControlData`

```cpp
void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}
```

* 뭔가 싶겠지만 아직은 그냥 넘어가자
* 바로 뒤에 v를 이용해 뷰를 바꿔보면서 실습 예정

* 우선 입력 매핑을 하나 만들자
    * Content\ArenaBattle\Input\Action
    * IA_Jump를 하나 복사해 IA_ChangeControl 생성
    * IMC_Default에 IA_ChangeControl를 등록하는 것을 잊지말자
    * 기존 IA_Move, IA_Look을 IA_ShoulderMove, IA_ShoulderLook으로 변경
    * IA_ShoulderMove를 하나 더 복사해 IA_QuaterMove 생성
    * IMC_Default도 IMC_Shoulder로 수정하고 복사해 IMC_Quater 생성
* ABCharacterPlayer 클래스에 입력 매핑 관련하여 수정할 부분이 많음 코드참조할 것.

* `void ChangeCharacterControl();` - v를 누르면 실행되는 함수
* `void SetCharacterControl(ECharacterControlType NewCharacterControlType);` - 실제로 동작은 여기서 한다.

```cpp
// 우선 v키 매핑은 여기서 한다.
void AABCharacterPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// ...

	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);
```

```cpp
void AABCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Quater)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quater);
	}
}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
    // CharacterControlManager는 CharacterBase에 선언된 Map임 참고.
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}
```

* 마지막으로 새로 생긴 QuaterMove는

```cpp
void AABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();
	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}
```