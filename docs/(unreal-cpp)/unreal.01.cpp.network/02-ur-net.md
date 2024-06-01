---
layout: default
title: "02. Log-in"
parent: "(Network)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/2)

---

## 로그 카테고리 지정

```cpp
// .h
DECLARE_LOG_CATEGORY_EXTERN(LogABNetwork, Log, All);
```

```cpp
// .cpp
DEFINE_LOG_CATEGORY(LogABNetwork);
IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, MyGame, "MyGame" );
```

* 이런식으로 로그 카테고리를 지정해준다.

---

## GameMode구현

```cpp
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase, public IABGameInterface
{
	// ...

    // 클라이언트의 접속 요청을 처리
        // PreLogin통과 후 Login 호출
        // 서버는 PreLogin이 없다.
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

    // 접속을 허용한 클라이언트의 컨트롤러 생성
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
    /*
        // Login한 플레이어 컨트롤러 생성
    APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
    */

    // 플레이어에 필요한 기본 설정을 모두 마무리
        // 인자로 컨틀롤러가 넘어옴을 확인
	virtual void PostLogin(APlayerController* NewPlayer) override;

    // 게임의 시작
    virtual void StartPlay() override;

    // 게임시작시(StartPlay) 모든 액터에서 호출
        // 모든 클라에서 호출이 된다는 말.
    virtual void BeginPlay() override;
```

---

## 게임 시작 후 과정, 게임 스테이트 관리

```
[리슨서버]                      [클라이언트]
게임모드
플레이어 컨트롤러0
플레이어 컨트롤러1 ----> (통보) 플레이어 컨트롤러0
게임 스테이트 ---------> (통보) 게임 스테이트
```

```cpp
AABGameMode::AABGameMode()
{
	// ...

    // AABGameMode에서 State 지정
	GameStateClass = AABGameState::StaticClass();
}
```

```cpp
class ARENABATTLE_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
    // 게임의 시작을 핸들링(BeginPlay)
	virtual void HandleBeginPlay() override;

protected:
// 이 함수는 멀티플레이어 게임에서 객체가 네트워크 상에 복제될 때 호출되며, 
// 특히 객체의 "BegunPlay" 상태가 네트워크 상의 다른 클라이언트들과 동기화될 때 사용
	virtual void OnRep_ReplicatedHasBegunPlay() override;
};
```