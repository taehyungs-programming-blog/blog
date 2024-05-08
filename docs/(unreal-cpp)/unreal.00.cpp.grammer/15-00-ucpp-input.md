---
layout: default
title: "15. enhanced input"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UE 4.X 버전의 input

* Project Settings -> Engine -> Input -> Action Mappings에서 하게된다.
    * 사실 이제 대부분 UE5니 크게 중요하진 않다 ㅎㅎ..

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp15-0-1.png"/>
</p>

* [Perplexity 🌟](https://www.perplexity.ai/search/Unreal-4-action-5_9gjXI9TQy0TyMcC2SoDA#4)

---

## UE 5.X enhanced input

* 두 가지 준비물이 필요함.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp15-0-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp15-0-3.png"/>
</p>

* 쓰기는 이렇게 쓴다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp15-0-4.png"/>
</p>

* C++로 해보자면 ...
    * [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/blob/1/Source/R1/Player/R1PlayerController.cpp)

```cpp
void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (const UMyInputData* InputData = UR1AssetManager::GetAssetByName<UMyInputData>("InputData"))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputData->InputMappingContext, 0);
		}
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const UMyInputData* InputData = UR1AssetManager::GetAssetByName<UMyInputData>("InputData"))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		auto Action1 = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Move);
		EnhancedInputComponent->BindAction(Action1, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);

		auto Action2 = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Turn);
		EnhancedInputComponent->BindAction(Action2, ETriggerEvent::Triggered, this, &ThisClass::Input_Turn);

		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
		//EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Turn);
	}
}

void AMyPlayerController::Input_Move(const FInputActionValue& InputValue)
{
	FVector2D MovementVector = InputValue.Get<FVector2D>();

	if (MovementVector.X != 0)
	{
		//FVector Direction = FVector::ForwardVector * MovementVector.X;
		//GetPawn()->AddActorWorldOffset(Direction * 50.f); // * DeltaTime

		FRotator Rotator = GetControlRotation();
		FVector Direction = UKismetMathLibrary::GetForwardVector(FRotator(0, Rotator.Yaw, 0));
		GetPawn()->AddMovementInput(Direction, MovementVector.X);
	}

	if (MovementVector.Y != 0)
	{
		//FVector Direction = FVector::RightVector * MovementVector.Y;
		//GetPawn()->AddActorWorldOffset(Direction * 50.f); // * DeltaTime

		FRotator Rotator = GetControlRotation();
		FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0, Rotator.Yaw, 0));
		GetPawn()->AddMovementInput(Direction, MovementVector.Y);
	}
}

void AMyPlayerController::Input_Turn(const FInputActionValue& InputValue)
{
	float Val = InputValue.Get<float>();
	AddYawInput(Val);
}

```