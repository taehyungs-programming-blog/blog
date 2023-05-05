---
layout: default
title: "01. 프로젝트 세팅"
parent: "(Unreal C++ Part2 - 게임만들어 보기)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_Basic_2/tree/1)

## 프로젝트 생성 + 기본환경 설정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_1_1.png"/>
</p>

* https://github.com/ideugu/UnrealProgrammingPart2/tree/1_1 내의 ArenaBattle폴더를 생성된 Content폴더에 복사한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_1_2.png"/>
</p>

* ArenaBattle\Maps내에 새로운 Maps을 생성해 보자.
    * Ctrl + n -> Basic
    * 바닥을 제거
    * ArenaBattle\Environment\Stage 폴더의 SM_SQUARE 복사
    * Player Start를 적당한 위치에 생성
    * 프로젝트 세팅에서 Map & Mode -> Editor Start Map, Game Basic Map을 위의 Map으로 지정

* GameMode 만들기
    * 메뉴바 -> Tools -> New C++ Class -> Game Mode Base로 생성
    * 생성 폴더는 `D:/Git/Unreal/ArenaBattle/Source/ArenaBattle/Game/` ArenaBattle아래 Game폴더를 새로 만들 것
    * 파일이름은 ABGameMode
    * 생성하면 에러가 발생한다. 당황하지 말고 `ArenaBattle.Build.cs`에 아래를 추가한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_1_3.png"/>
</p>

```csharp
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ArenaBattle : ModuleRules
{
	public ArenaBattle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // ArenaBattle을 Include해달라는 명령으로 받아들이자.
		PublicIncludePaths.AddRange(new string[] { "ArenaBattle" });
```

* UE Editor에서 Ctrl + Alt + F11로 다시 빌드를 시도한다.

---

* Player Controller Class 추가
    * Tools -> New C++ Class -> Player Controller로 생성
    * `D:/Git/Unreal/ArenaBattle/Source/ArenaBattle/Player/` 내에 생성
    * 이름은 ABPlayerController
* Character Class 추가
    * Tools -> New C++ Class -> Character로 생성
    * `D:/Git/Unreal/ArenaBattle/Source/ArenaBattle/Character/`
    * 이름은 ABCharacterBase
* ABCharacterBase를 상속받은 
    * ABCharacterPlayer도 생성
* 기본 캐릭서 생성하기
    * Content Browser -> Add -> Add Content to the Project -> Third Person 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_1_4.png"/>
</p>

* World Settings에서 GameMode Override를 ABGameMode로 변경해야하는데 이걸 C++로 해보려한다. Visual Studio를 열자!
    * 그전에 UE Editor에서 World Settings -> GameMode Override를 ABGameMode로 변경해놓자.
    * 다시 Visual Studio로!

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_1_5.png"/>
</p>

```cpp
// ABGameMode.h

UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
// 오직 생성자만 만들었다.
public:
	AABGameMode();
	
};
```

```cpp
#include "Game/ABGameMode.h"
#include "ABGameMode.h"
#include "Player/ABPlayerController.h"

AABGameMode::AABGameMode()
{
    // GameMode 생성시 PlayerControllerClass를 지정해준다.
	PlayerControllerClass = AABPlayerController::StaticClass();
}
```

* 빌드! Ctrl+Alt+F11

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_1_6.png"/>
</p>

* 요렇게 변함!

---

* 이제 Default Pawn Class 정보를 넣어보자.
    * ThirdPerson의 Blueprints의 BP_ThirdPersonCharacter를 사용할 예정인데 우클릭 후 Copy Reference를 누르면 아래 정보를 얻을 수 있다.
    * `/Script/Engine.Blueprint'/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter'`
    * 이걸 이용해 C++에서 Default Pawn Class를 지정해보자.

```cpp
AABGameMode::AABGameMode()
{
    // 마지막 _C가 의문스러울 수 있는데 Class정보만 얻어온다는 명령이라고 생각하자
	static ConstructorHelpers::FClassFinder<APawn> ThirdPersonClassRef(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	if (ThirdPersonClassRef.Class)
	{
		DefaultPawnClass = ThirdPersonClassRef.Class;
	}

	PlayerControllerClass = AABPlayerController::StaticClass();
}
```

* 빌드! Ctrl+Alt+F11

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_1_7.png"/>
</p>

* 요렇게 변함!

---

* 하나만 더 해보자.
    * 현재 불편한 점이 게임시작시 마우스로 게임화면을 클릭해 줘야 마우스가 사라지며 키보드입력을 받을수 있다는 점이다.
    * 게임이 시작하자마자 입력을 바로 받게 해보자.

```cpp
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
};

```

```cpp
void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
```

* 빌드 (Ctrl+Alt+F11) 후 테스트 해보면 마우스가 자동으로 잡히며 마우스를 빼고 싶으면 F11을 누르면 된다.

---

* 마지막! 하나만 더!

```cpp
#include "Game/ABGameMode.h"
#include "ABGameMode.h"

// ABPlayerController.h가 영... 마음에 안든다!
#include "Player/ABPlayerController.h"

AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> ThirdPersonClassRef(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	if (ThirdPersonClassRef.Class)
	{
		DefaultPawnClass = ThirdPersonClassRef.Class;
	}


    PlayerControllerClass = AABPlayerController::StaticClass();
}
```

```cpp
#include "Game/ABGameMode.h"
#include "ABGameMode.h"

AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> ThirdPersonClassRef(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	if (ThirdPersonClassRef.Class)
	{
		DefaultPawnClass = ThirdPersonClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ArenaBattle.ABPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}
```