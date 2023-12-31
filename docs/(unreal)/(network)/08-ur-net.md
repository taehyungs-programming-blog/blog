---
layout: default
title: "08. RPC(Remote Procedure Call)란?"
parent: "(Unreal Network)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/8)

## Remote Procedure Call

* 네트워크를 통해 **다른 컴퓨터 또는 게임 세션의 객체에 있는 함수를 호출**하는 방법입니다. 
    * 이름 그대로 원격 프로시저(함수)를 콜 한다는 뜻.

* Unreal Engine에서는 주로 세 가지 유형의 RPC가 있습니다:
    * **Server RPCs**: 
        * **클라이언트가 서버에서 함수를 실행**하도록 요청할 때 사용합니다. 
        * 예를 들어, 플레이어가 캐릭터를 이동하려고 할 때, **이동 명령은 클라이언트에서 시작되어 서버에서 실행**됩니다.
    * **Client RPCs**: 
        * **서버가 클라이언트에서 함수를 실행**하도록 요청할 때 사용합니다. 
        * 예를 들어, 서버가 특정 클라이언트에게만 상태 업데이트를 보내거나 알림을 전달할 때 사용됩니다.
    * **Multicast RPCs**: 
        * **서버가 모든 클라이언트에게 함수 실행**을 동시에 방송할 때 사용됩니다. 
        * 예를 들어, 특정 이벤트가 발생했을 때 모든 클라이언트에게 알리고 싶을 때 유용합니다.

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class MYGAME_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AMyCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Function to handle the speak action
    void HandleSpeak();

    // Server RPC function declaration
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSpeak();
    void ServerSpeak_Implementation();
    bool ServerSpeak_Validate();
};
```

```cpp
#include "MyCharacter.h"

AMyCharacter::AMyCharacter()
{
    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind the speak action
    PlayerInputComponent->BindAction("Speak", IE_Pressed, this, &AMyCharacter::HandleSpeak);
}

void AMyCharacter::HandleSpeak()
{
    // Call the ServerSpeak function when the player presses the speak key
    if (Role < ROLE_Authority)
    {
        ServerSpeak();
    }
}

void AMyCharacter::ServerSpeak_Implementation()
{
    // This is the code that gets executed on the server when the player speaks
    GLog->Log("Hello!");
}

bool AMyCharacter::ServerSpeak_Validate()
{
    // Validate the request here if necessary
    return true; // Request is valid
}

```

---

