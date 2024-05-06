---
layout: default
title: "04. ALyraGameMode"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
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

```cpp
class ALyraGameMode : public AModularGameModeBase
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-4-1.png"/>
</p>

* Unreal의 GameMode는 아래와 같이 타이트하게 설정해 줘야한다.

```cpp
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

## (Tips) Level의 이주(Migrate) 방법

* 우클릭 후 Asset Actions -> Migrate ...
    * 필요한 Asset까지 같이 따라온다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-4-2.png"/>
</p>

---

## (Tips) 뭔가 에러가 난다 싶으면 ?

* .ini에서 뭘 안가져왔나 확인하라.