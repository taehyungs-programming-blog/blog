---
layout: default
title: "14. Pawn"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 개념

* **Pawn**은 게임 세계 안에서 플레이어나 AI가 제어할 수 있는 모든 액터의 기본 클래스를 지칭합니다. 
    * 이 클래스는 게임 내에서 캐릭터의 이동이나 상호작용 등을 관리하는 데 사용됩니다. 
    * Pawn은 일반적으로 사용자 입력이나 AI 스크립트를 받아들여 그 입력에 따라 동작하도록 설계되어 있습니다.

* Pawn의 주요 기능:
    * 입력 처리: Pawn은 키보드, 마우스, 게임패드 등의 입력 장치로부터 입력을 받을 수 있습니다. 이를 통해 사용자는 Pawn을 제어할 수 있습니다.
    * 이동 및 물리: Pawn은 이동 로직과 물리적 상호작용을 처리할 수 있습니다. 예를 들어, 캐릭터의 걷기, 달리기, 점프 등의 기본 이동 메커니즘을 구현할 수 있습니다.
    * 카메라 제어: 많은 Pawn 클래스에서는 플레이어의 시점을 결정하는 카메라 제어 기능을 포함하고 있습니다. 이를 통해 게임 개발자는 플레이어의 시각 경험을 세밀하게 조정할 수 있습니다.
    * AI 행동: AI Pawn은 자동으로 환경을 탐색하고, 목표를 추구하며, 플레이어나 다른 게임 요소와 상호작용할 수 있도록 프로그래밍될 수 있습니다.
    * 확장성: Pawn은 다양한 방식으로 확장이 가능합니다. 예를 들어, 복잡한 캐릭터들을 위해 "Character" 클래스가 Pawn을 상속받아 추가적인 기능과 메커니즘을 제공합니다.

---

## Actor랑은 무슨 차이?

* **Actor**
    * 기본 정의: Actor는 Unreal Engine에서 모든 게임 오브젝트의 기본 클래스입니다. 건물, 차량, 가구, 풍경 등 게임 내의 거의 모든 요소가 Actor를 상속받습니다.
    * 목적: Actor는 게임 세계 내에서의 위치, 회전, 크기 등의 기본적인 공간적 특성을 관리합니다. 또한, Actor는 이벤트, 시간의 흐름에 따른 변경 사항, 그리고 상호작용을 처리하는 기본적인 기능을 제공합니다.
    * 사용 사례: Actor는 정적인 배경 오브젝트에서부터 동적인 효과 오브젝트, 게임 로직을 구현하는 오브젝트에 이르기까지 광범위하게 사용됩니다.
* **Pawn**
    * 기본 정의: **Pawn은 Actor의 하위 클래스로**, 플레이어나 AI에 의해 직접 제어될 수 있는 Actor를 의미합니다.
    * 목적: Pawn의 주요 목적은 사용자 입력 또는 AI 제어 로직을 받아들여 게임 세계에서의 행동을 구현하는 것입니다. 이를 통해 플레이어가 게임 캐릭터를 조종하거나 AI가 캐릭터를 제어하는 데 필요한 기능을 제공합니다.
    * 사용 사례: 주로 캐릭터, 차량 등 플레이어가 조작할 수 있는 동적인 요소들이 Pawn을 상속받아 사용됩니다.
* 결론적으로, 모든 Pawn은 Actor이지만, 모든 Actor가 Pawn은 아닙니다. Actor는 게임 오브젝트의 가장 기본적인 형태를 제공하고, Pawn은 그 위에서 플레이어와 AI의 제어를 가능하게 하는 추가적인 기능을 갖춘 특수한 Actor입니다. **Pawn은 게임 개발에서 Actor의 기능을 확장하여 보다 상호작용적이고 동적인 요소를 구현할 때 사용됩니다.**

---

## GameMode와 Pawn

```cpp
ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    // ...

    // 요렇게 지정을 한다.
	DefaultPawnClass = ALyraCharacter::StaticClass();
	HUDClass = ALyraHUD::StaticClass();
}
```

---

## PlayerController와 Pawn

```cpp
ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    // ...
    // 역시 GameMode에서 등록.
        // Pawn의 Pocessess시 등록된다.
	PlayerControllerClass = ALyraPlayerController::StaticClass();
```

