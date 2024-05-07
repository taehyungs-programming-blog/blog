---
layout: default
title: "05-01. GameMode와 GameInstance에 관해"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## GameMode

* [Docs 🌟](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/gameplay-framework-in-unreal-engine?application_version=5.3) : 조금 이해를 돕는다.

* GameMode는 게임의 규칙을 정의합니다. 
* 이 클래스는 주로 특정 게임 레벨이나 맵에서 어떻게 플레이어가 게임에 참여하고, 게임이 어떻게 진행되며, 승리 조건은 무엇인지 등을 결정합니다. GameMode는 다음과 같은 기능을 담당합니다:

* 주요기능
    * 플레이어 스폰: 플레이어가 게임에 참여할 때 어디에, 어떻게 스폰할지를 결정합니다.
    * 게임 규칙: 게임의 승리 조건, 시간 제한, 점수 계산 등 게임의 기본적인 규칙을 설정합니다.
    * 게임 플로우: 게임 시작, 종료, 재시작 등 게임의 전반적인 플로우를 관리합니다.
    * 플레이어 상호작용: 플레이어 간의 상호작용이나 게임 로직에 따른 이벤트 처리를 담당합니다.

* GameMode는 각 레벨 또는 맵에 특화되어 있으며, 맵이 로드될 때 마다 새로 생성됩니다. 따라서, 다양한 맵이나 레벨에서 다른 게임 규칙을 적용하고 싶을 때 유용합니다.

```cpp
// Example
ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    // GameMode는 이런애들을 관리해 준다.
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

## GameInstance

* GameInstance는 게임의 전체 실행 동안 존재하는 객체로, 여러 레벨이나 세션에 걸쳐 데이터를 유지하는 데 사용됩니다. 이 클래스는 다음과 같은 목적으로 주로 사용됩니다:

* 주요기능
    * 데이터 지속성: 다른 레벨로의 전환 시에도 플레이어 정보나 게임 설정 같은 데이터를 유지합니다.
    * 네트워킹: 온라인 게임에서 세션 관리나 네트워크 통신을 처리하는 데 중요한 역할을 합니다.
    * 글로벌 설정: 전체 게임에 걸쳐 적용되어야 할 설정이나 상태 관리를 합니다.

* GameInstance는 게임이 처음 시작될 때 생성되어 게임이 종료될 때까지 지속되므로, 레벨 로딩과는 독립적으로 동작합니다. 따라서, 게임 세션 전체를 통틀어 유지해야 하는 정보를 다루는 데 적합합니다.

---

## 정리

* GameMode: **맵별로 새로 생성**되며 게임의 규칙과 플로우를 정의합니다. 레벨이 바뀌면 새로운 GameMode가 생성됩니다.
* GameInstance: **게임 세션 전체를 통틀어 유지**되며, 게임의 전반적인 데이터와 설정, 네트워크 세션을 관리합니다.
이 두 클래스를 통해 Unreal Engine은 맵별 설정과 전체 게임 흐름을 유연하게 관리할 수 있습니다, 게임의 다양한 측면을 효과적으로 분리하고 관리할 수 있도록 해줍니다.