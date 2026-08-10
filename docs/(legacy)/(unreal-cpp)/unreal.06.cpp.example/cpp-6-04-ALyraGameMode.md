---
layout: default
title: "04. ALyraGameMode"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/LyraClone/tree/1)

---

* 띠용? 우리가 기존에 알던 GameMode가 아니고 `AModularGameModeBase`네?
	* 이건 차차 설명하겠다.
	* 대충 GameMode말고 AModularGameModeBase를 써서 GameMode를 대체한다고 생각해 두자

```cpp
class ALyraGameMode : public AModularGameModeBase
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-4-1.png"/>
</p>

* Unreal의 GameMode는 아래와 같이 타이트하게 설정해 줘야한다.

```cpp
// 생긴건 GameMode와 유사함
ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = ALyraGameState::StaticClass();
	GameSessionClass = ALyraGameSession::StaticClass();
	PlayerControllerClass = ALyraPlayerController::StaticClass();
	ReplaySpectatorPlayerControllerClass = ALyraReplayPlayerController::StaticClass();
	PlayerStateClass = ALyraPlayerState::StaticClass();
	DefaultPawnClass = ALyraCharacter::StaticClass();
	HUDClass = ALyraHUD::StaticClass();
}
```

---

## 그래도 궁금해 할지 모르니 AGameMode와 AModularGameModeBase에 대해 설명하자면

### AGameMode

* AGameMode는 Unreal Engine의 기본 게임 모드 클래스입니다. 
* 게임의 규칙, 승리 조건, 플레이어 시작 지점, 기본 플레이어 캐릭터 및 HUD 등을 정의합니다. AGameMode는 일반적으로 다음과 같은 역할을 합니다:
	* 게임 규칙 설정: 게임의 기본 규칙과 동작 방식을 정의합니다.
	* 플레이어 관리: 플레이어의 시작 지점을 설정하고, 플레이어의 기본 캐릭터를 스폰합니다.
	* 승리 및 패배 조건 설정: 게임의 승리 및 패배 조건을 정의합니다.
	* HUD 및 UI 관리: 게임 내 HUD 및 UI를 관리합니다.

### AModularGameModeBase

* AModularGameModeBase는 AGameMode에서 파생된 클래스이며, **모듈화된 게임 모드**를 구현하기 위해 사용됩니다. 
* 이 클래스는 특히 게임의 각기 다른 부분을 모듈로 분리하여 보다 유연하고 확장 가능한 게임 모드를 만들기 위한 목적으로 설계되었습니다. AModularGameModeBase는 일반적으로 다음과 같은 특징을 가집니다:
	* 모듈화된 구조: 게임 모드를 여러 모듈로 분리하여 관리할 수 있습니다. 이를 통해 특정 기능이나 로직을 별도의 모듈로 구현하고 필요에 따라 교체하거나 확장할 수 있습니다.
	* 확장성: 기존의 AGameMode보다 더 유연하고 확장 가능한 구조를 제공하여, 다양한 게임 모드나 규칙을 보다 쉽게 구현할 수 있습니다.
	* 구성 요소 관리: 게임 모드의 구성 요소를 보다 체계적으로 관리할 수 있어, 게임의 특정 기능을 재사용하거나 독립적으로 테스트하기 용이합니다.

### AModularGameModeBase를 왜 쓸까?

* `AGameMode`를 쓴 예시를 들어보자.

```cpp
class AMyGameMode : public AGameMode
{
public:
    AMyGameMode()
    {
        // 기본 게임 모드 설정
        DefaultPawnClass = AMyCharacter::StaticClass();
        PlayerStateClass = AMyPlayerState::StaticClass();
        HUDClass = AMyHUD::StaticClass();
    }

    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        // 게임 시작 로직
    }

    virtual void PostLogin(APlayerController* NewPlayer) override
    {
        Super::PostLogin(NewPlayer);
        // 플레이어가 접속했을 때 처리할 로직
    }
};
```

```cpp
// MyModularGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "ModularGameModeBase.h"
#include "MyModularGameMode.generated.h"

UCLASS()
class MYGAME_API AMyModularGameMode : public AModularGameModeBase
{
    GENERATED_BODY()

public:
    AMyModularGameMode();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void StartGame();
    void EndGame();

private:
    bool bIsGameOver;
};
```

```cpp
// MyModularGameMode.cpp
AMyModularGameMode::AMyModularGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsGameOver = false;

    // 컴포넌트 추가
    AddComponentByClass(UMyPlayerSpawnerComponent::StaticClass());
}

void AMyModularGameMode::BeginPlay()
{
    Super::BeginPlay();
    StartGame();
}

void AMyModularGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsGameOver)
    {
        EndGame();
    }
}

void AMyModularGameMode::StartGame()
{
    // 게임 시작 로직
}

void AMyModularGameMode::EndGame()
{
    // 게임 종료 로직
}
```

---

## (Tips) Level의 이주(Migrate) 방법

* 우클릭 후 Asset Actions -> Migrate ...
    * 필요한 Asset까지 같이 따라온다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-4-2.png"/>
</p>

---

## (Tips) 뭔가 에러가 난다 싶으면 ?

* .ini에서 뭘 안가져왔나 확인하라.