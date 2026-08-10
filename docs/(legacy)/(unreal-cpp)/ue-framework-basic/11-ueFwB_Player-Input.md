---
layout: default
title: "11. Player Input 처리 방법"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 세 클래스가 관계하게 된다.
    * UGameViewportClient, UPlayerInput, UInputComponent

## UGameViewportClient

* 역할: 최상위 입력 진입점. OS/플랫폼 레벨에서 들어온 Raw Input(키보드, 마우스, 패드 등) 이벤트를 받음
    * 여기서 입력이 어떤 Player에게 전달될지 결정
* Viewport ↔ PlayerController 연결고리로, 여러 Local Player가 있을 수 있는 split-screen 같은 환경에서도 입력 분배

## UPlayerInput

* 역할: PlayerController에 종속되어, 바인딩된 입력(Action/Axis Mapping) 을 해석
    * 즉, "W 키 → MoveForward", "Space → Jump" 같은 매핑을 실제 실행 가능한 InputEvent로 변환
* 키 상태, 축 값(accumulated axis values) 등을 보관/업데이트

## UInputComponent

* 역할: 실제 게임 로직과 입력 이벤트를 연결
    * Pawn, Character, Actor, Controller 등은 자신만의 UInputComponent를 가질 수 있고, 여기서 특정 바인딩을 설정
* 예를 들어 "Jump"라는 ActionMapping이 발생했을 때 ACharacter::Jump()를 호출하도록 연결하는 식입니다.

```
[OS/플랫폼 Input] 
   ↓
UGameViewportClient
   ↓
UPlayerInput (PlayerController 소속)
   ↓
UInputComponent (Pawn/Character/Actor 소속)
   ↓
게임 로직 실행
```

---

## Example

```cpp
// MyCharacter.cpp

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // "Jump"라는 ActionMapping이 눌렸을 때 Jump 실행
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

    // "MoveForward"라는 AxisMapping 값으로 캐릭터 이동
    PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
}

void AMyCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

```