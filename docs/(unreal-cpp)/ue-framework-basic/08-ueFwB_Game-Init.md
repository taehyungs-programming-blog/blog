---
layout: default
title: "08. 게임이 초기화 되는 과정에 관계하는 클래스 정리"
parent: "(Unreal Framwork의 이해)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 전체 흐름(요약)

> 아래 순으로 생성된다고 생각하면 좋다

* UGameInstance (프로젝트 생명주기와 같음)
* UWorld (맵 로드되며 생성)
* UGameViewportClient (클라이언트 I/O 창)
* ULocalPlayer (클라 플레이어 슬롯)
* AGameModeBase / AGameMode (서버 전용, 월드마다)
* AGameStateBase / AGameState (모두 존재, 레플리케이션 허브)
* APlayerController (플레이어별 컨트롤러)
* APlayerState (플레이어 상태; 네트워크 동기화)
* APlayerCameraManager
* UPlayerInput
* APawn / ACharacter (실체; Controller가 Possess)

> 멀티플레이: GameMode는 서버에만 존재, GameState/PlayerState는 서버→클라 복제, PlayerController는 서버/로컬 모두, Pawn은 보통 서버 스폰→복제.

---

* 이제 각 클래스의 대략적 설명

## UGameInstance

* 역할: 게임 전체 수명과 함께하는 싱글턴성 컨테이너. 세션/로비/공용 매니저/서브시스템 초기화.
* 생성 시점: 실행 시작 시 1회. 맵 전환 간에도 유지.
* 주요 콜백: Init(), OnStart(), Shutdown() / (BP: Event Init/Shutdown)

```cpp
// MyGameInstance.h
UCLASS()
class UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()
protected:
    virtual void Init() override;
    virtual void OnStart() override;
    virtual void Shutdown() override;
};

// MyGameInstance.cpp
void UMyGameInstance::Init()
{
    UE_LOG(LogTemp, Log, TEXT("[GI] Init"));
    // 싱글톤 매니저/서브시스템 초기화
}
void UMyGameInstance::OnStart()
{
    UE_LOG(LogTemp, Log, TEXT("[GI] OnStart"));
}
void UMyGameInstance::Shutdown()
{
    UE_LOG(LogTemp, Log, TEXT("[GI] Shutdown"));
}
```

---

## UWorld

* 역할: 레벨(맵)과 액터들을 담는 컨테이너.
* 생성 시점: 맵 로드 시. Persistent/Streaming 레벨 구조 포함.
* 관련 콜백(레벨): UGameInstance::OnWorldChanged, AActor::BeginPlay()가 여기서부터 돌기 시작.

> 월드 전환 시에는 GI 유지 + 월드가 교체됩니다.

---

## UGameViewportClient

* 역할: 렌더링 뷰포트/입출력 라우팅/슬릿스크린 처리.
* 생성 시점: 클라이언트 실행 시.
* 주요 콜백: Init(), Tick(), InputKey().

```cpp
UCLASS()
class UMyViewportClient : public UGameViewportClient
{
    GENERATED_BODY()
public:
    virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice) override
    {
        Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
        UE_LOG(LogTemp, Log, TEXT("[Viewport] Init"));
    }
    virtual void Tick(float DeltaSeconds) override
    {
        Super::Tick(DeltaSeconds);
    }
};

```

---

## ULocalPlayer

* 역할: 로컬 입력 소유, PlayerController 생성 트리거. 스플릿스크린이면 여러 개.
* 생성 시점: 로컬에서 플레이어 추가 시(기본 1개).
* 중요 포인트: GetPlayerController, 로컬 옵션/SaveGame 접근 포인트로 자주 사용.

```cpp
// 로컬 플레이어 확보 예시
if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
{
    if (ULocalPlayer* LP = Viewport->GetGameInstance()->GetFirstGamePlayer())
    {
        APlayerController* PC = LP->GetPlayerController(GetWorld());
        UE_LOG(LogTemp, Log, TEXT("[LocalPlayer] PC = %s"), *GetNameSafe(PC));
    }
}

```

---

## AGameModeBase / AGameMode (서버 전용)

* 역할: 규칙 관리자. 매치 흐름, Pawn 스폰, 팀 규칙, 승패 판정 등.
* 생성 시점: 월드 로드 시 서버에서만. 맵마다 다를 수 있음.
* 주요 콜백: InitGame(), InitGameState(), StartPlay(), PostLogin(), HandleStartingNewPlayer(), RestartPlayer()
* 중요 함수: SpawnDefaultPawnFor(), ChoosePlayerStart().

```cpp
UCLASS()
class AMyGameMode : public AGameMode
{
    GENERATED_BODY()
public:
    AMyGameMode();
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void StartPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
};

AMyGameMode::AMyGameMode()
{
    DefaultPawnClass = AMyCharacter::StaticClass();
    PlayerControllerClass = AMyPlayerController::StaticClass();
    GameStateClass = AMyGameState::StaticClass();
    PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    UE_LOG(LogTemp, Log, TEXT("[GM] InitGame: %s"), *MapName);
    Super::InitGame(MapName, Options, ErrorMessage);
}

void AMyGameMode::StartPlay()
{
    UE_LOG(LogTemp, Log, TEXT("[GM] StartPlay"));
    Super::StartPlay();
}

void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
    UE_LOG(LogTemp, Log, TEXT("[GM] PostLogin: %s"), *GetNameSafe(NewPlayer));
    Super::PostLogin(NewPlayer);
}

APawn* AMyGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    // 시작 위치/커스텀 Pawn 스폰 커스터마이즈
    return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}
```

### GameInstance Vs GameMode

* UGameInstance
    * 수명주기: 게임 실행 시작 → 종료까지 한 번만 존재. 맵이 바뀌어도 유지됨.
    * 역할: 앱 전체에 걸친 “전역 컨텍스트”를 관리.
        * 로그인 상태, 세션 매니저, 서버 브라우저, 퍼시스턴트 데이터 등을 담음.
        * 로비/매치 사이를 오갈 때도 살아있으므로 게임 전체 단위 관리에 적합.
* AGameMode (및 GameModeBase)
    * 수명주기: 월드(맵) 단위로 존재.
        * 맵 로드 시 새로 생성 → 해당 맵 끝나면 파괴.
        * 서버에서만 존재하고, 클라는 GameState를 통해 간접적으로 본다.
    * 역할: 규칙/흐름(라운드 시작, 승패 판정, 스폰 위치, 팀 배정 등)을 정의.
    * 기본 Pawn/Controller/State 클래스를 지정.
    * 플레이어 입장(PostLogin), 리스폰(SpawnDefaultPawnFor) 등 매치 규칙 중심.

* 예시 시나리오
    * GameInstance: 유저가 로비에서 캐릭터를 선택 → 서버 찾기 → 매치 입장 → 매치 끝나고 다시 로비 복귀.
        * 이 모든 동안 GameInstance는 살아있어서 선택 정보, 세션 상태, 서버 주소를 계속 기억.
    * GameMode: 매치 시작 시 맵이 로드되면서 새로운 GameMode가 생성 → 라운드 규칙, 스폰, 점수 관리.
        * 매치 끝나고 로비로 돌아가면 해당 GameMode는 파괴되고, 새로운 로비 GameMode(혹은 아예 없음)가 생김.

---

## APlayerController

* 역할: 입력 처리·카메라 제어·UI 조작·서버 RPC. 로컬이면 입력을 받고, 서버 측 인스턴스는 권한 로직 처리.
* 생성 시점: 플레이어 참가 시.
* 주요 콜백: BeginPlay(), SetupInputComponent(), Tick(), OnPossess().

```cpp
UCLASS()
class AMyPlayerController : public APlayerController
{
    GENERATED_BODY()
protected:
    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        UE_LOG(LogTemp, Log, TEXT("[PC] BeginPlay (Local=%d)"), IsLocalController());
    }
    virtual void SetupInputComponent() override
    {
        Super::SetupInputComponent();
        InputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerController::OnJump);
    }
    void OnJump()
    {
        if (APawn* P = GetPawn()) { /* 점프 트리거 */ }
    }
    virtual void OnPossess(APawn* InPawn) override
    {
        Super::OnPossess(InPawn);
        UE_LOG(LogTemp, Log, TEXT("[PC] Possess %s"), *GetNameSafe(InPawn));
    }
};

```

---

## APlayerState

* 역할: 플레이어의 상태(닉네임/팀/점수/핑 등) 복제. 로비/스코어보드 표시.
* 생성 시점: 컨트롤러 생성 즈음, 서버에서 생성되어 클라에 복제.
* 포인트: 소유자 불문 공용 읽기 가능(클라 표시용), 쓰기는 권한이 있는 쪽(보통 서버).

```cpp
UCLASS()
class AMyPlayerState : public APlayerState
{
    GENERATED_BODY()
public:
    UPROPERTY(ReplicatedUsing=OnRep_DisplayName)
    FText DisplayName;

    UFUNCTION()
    void OnRep_DisplayName(){ /* UI 갱신 */ }

    // 서버에서 설정하는 RPC
    UFUNCTION(Server, Reliable)
    void ServerSetDisplayName(const FText& NewName){ DisplayName = NewName; }
};
```

---

## APlayerCameraManager

* 역할: 시점/포스트프로세스/카메라 흔들림·FOV 제어.
* 생성 시점: 컨트롤러당 1개(기본 자동 생성).
* 주요 콜백: UpdateCamera().

```cpp
UCLASS()
class AMyCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()
public:
    virtual void UpdateCamera(float DeltaTime) override
    {
        Super::UpdateCamera(DeltaTime);
        // FOV/Lerp/Shake 적용
    }
};

```

---

## UPlayerInput

* 역할: 프레임마다 입력 축/액션을 수집해 Controller로 전달.
* 생성 시점: PlayerController 생성 시 내부적으로.
* 사용 포인트: 실제 바인딩은 대개 SetupInputComponent()에서 처리.

---

## APawn / ACharacter

* 역할: 플레이어가 Possess하는 실체(캐릭터/차량/드론 등). 이동·충돌·애니메이션.
* 생성 시점: 보통 서버의 GameMode가 스폰 → 네트워크 복제 → 각 클라의 PlayerController가 Possess.
* 주요 콜백: PossessedBy(), UnPossessed(), SetupPlayerInputComponent(), BeginPlay().

```cpp
UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()
protected:
    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        UE_LOG(LogTemp, Log, TEXT("[Pawn] BeginPlay (Authority=%d)"), HasAuthority());
    }
    virtual void PossessedBy(AController* NewController) override
    {
        Super::PossessedBy(NewController);
        UE_LOG(LogTemp, Log, TEXT("[Pawn] PossessedBy %s"), *GetNameSafe(NewController));
    }
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override
    {
        PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
    }
    void MoveForward(float Value){ AddMovementInput(GetActorForwardVector(), Value); }
};

```

> 이제 이 과정을 실제 코드로 보자(다음강)