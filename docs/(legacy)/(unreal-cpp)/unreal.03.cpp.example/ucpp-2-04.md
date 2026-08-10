---
layout: default
title: "04. Character Controll 관련 옵션들"
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

* 다양한 방법으로 캐릭터를 컨트롤 해보자
* 우선, 현재는 어떻게 구성이 되어있나 보자.

---

### Look 함수

```cpp
void AABCharacterPlayer::Look(const FInputActionValue& Value)
{
    // 우선 내가 바라보는 방향의 Vector를 받아온다.
	FVector2D LookAxisVector = Value.Get<FVector2D>();

    // 로그를 찍어보면 마우스 회전변화량이 들어옴을 확인할 수 있다.
    UE_LOG(LogTemp, Warning, TEXT("Look input received: X=%f, Y=%f"), LookAxisVector.X, LookAxisVector.Y);

    // 변화량을 입력시 Controller의 회전값을 변경한다.
    // Yaw가 캐릭터 기준 좌/우
	AddControllerYawInput(LookAxisVector.X);
    // Pitch가 캐릭터 기준 상/하
	AddControllerPitchInput(LookAxisVector.Y);
}
```

* Look 함수는 마우스 입력으로 부터 받아온 회전 값을 바탕으로 컨트롤러의 컨트롤 회전을 설정한다.
    * 함수 이름이 좀;;;

```
# 인게임화면에서 (`)를 누르면 Cmd창을 띄울수 있음
$ DisplayAll PlayerController ControlRotation
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_4_1.png"/>
</p>

---

### Move 함수

* Move 함수는 캐릭터가 이동시 WSAD어느 키를 눌렀나 나온다

```cpp
void AABCharacterPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

    // 만약 A를 누르면 Y=-1이 나오는 이런식
    UE_LOG(LogTemp, Warning, TEXT("Look input received: X=%f, Y=%f"), MovementVector.X, MovementVector.Y);

    // 캐릭터의 Rotation정보를 기반으로
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

    // X축이 Forawrd, Y축이 Right
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    // 들어온 값 만큼 Move!
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}
```

---

## Character의 Pawn설정중 알아야 할 것!

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_4_2.png"/>
</p>

* Use Controller Rotation Pitch/Yaw/Roll - 컨트롤러에 저장된 Control Rotation 값에 Pawn의 Rotation을 맞출지 여부이다.
* 테스트 삼아 Yaw를 On하고 테스트 해보자.
    * Default Pawn Class 수정하는것 잊지말기
* 카메라 회전을 캐릭터가 그대로 따라가게 된다.

---

## Spring Arm Controll Option

* CameraBoom
    * Use Pawn Control Rotation - 카메라가 Pawn의 Rotation을 따라가는지 여부
* Camera Collision
    * 카메라와 캐릭터 사이 장애물이 있을시 카메라가 장애물을 피하게 할지 여부

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_4_3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_4_4.png"/>
</p>

---

## Character Movement Option

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_4_5.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_4_6.png"/>
</p>

* 이동관련
    * Movement Mode : None, Walking, Falling
    * 이동 모드에서의 이동 수치 : WaxWalkingSpeed
    * 폴링 모드에서 점프 수치 : JumpZVelocity
* 회전관련
    * Rotation Rate : 회전 속도
    * Use Controller Desired Rotation : 목표 회전을 정하고 지정한 속도로 회전
    * Orient Rotation To Movement : 캐릭터 이동 방향으로 회전
    * Pawn의 회전 옵션과 충돌 주의!

