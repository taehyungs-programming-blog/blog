---
layout: default
title: "02-08. PendingNetGame"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## PendingNetGame란?

* 클라이언트가 서버에 접속하는 과정에서 사용되는 임시 상태 객체

```cpp
// 1. 클라이언트 접속 시도시 생성
WorldContext.PendingNetGame = NewObject<UPendingNetGame>();

// 2. 초기화 및 네트워크 드라이버 설정
WorldContext.PendingNetGame->Initialize(URL);
WorldContext.PendingNetGame->InitNetDriver();

// 3. 서버로부터 맵 정보 수신 후 실제 World로 전환
// PendingNetGame은 이후 제거됨
```

---

## 호출순서

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_1_2.png"/>
</p>

```cpp
// UEngine::Browse - 클라이언트가 서버 접속을 시도할 때
virtual EBrowseReturnVal::Type Browse(FWorldContext& WorldContext, FURL URL, FString& Error)
{
    if (URL.IsInternal() && GIsClient)
    {
        // 새로운 PendingNetGame 생성
        WorldContext.PendingNetGame = NewObject<UPendingNetGame>();
        
        // URL 정보로 초기화 (서버 주소 등)
        WorldContext.PendingNetGame->Initialize(URL);
        
        // NetDriver 초기화 및 서버 연결 시도
        WorldContext.PendingNetGame->InitNetDriver();

        return EBrowseReturnVal::Pending;
    }
    return EBrowseReturnVal::Failure;
}
```

```cpp
// UEngine::TickWorldTravel - 매 프레임마다 호출
virtual void TickWorldTravel(FWorldContext& Context, float DeltaSeconds)
{
    if (Context.PendingNetGame)
    {
        // 네트워크 업데이트 처리
        Context.PendingNetGame->Tick(DeltaSeconds);

        // 서버로부터 Welcome 메시지를 받고 성공적으로 연결된 경우
        if (Context.PendingNetGame->bSuccessfullyConnected && 
            !Context.PendingNetGame->bSentJoinRequest && 
            !Context.PendingNetGame->bLoadedMapSuccessfully)
        {
            // 서버가 지정한 맵 로드 시도
            FString Error;
            const bool bLoadedMapSuccessfully = LoadMap(
                Context, 
                Context.PendingNetGame->URL, 
                Context.PendingNetGame, 
                Error
            );

            // 맵 로드 결과 처리
            if (!Context.PendingNetGame->LoadMapCompleted(this, Context, bLoadedMapSuccessfully, Error))
            {
                BrowseToDefaultMap(Context);
                BroadcastTravelFailure(Context.World(), ETravelFailure::LoadMapFailure, Error);
            }
        }

        // 맵 로드가 성공적으로 완료된 경우
        if (Context.PendingNetGame && Context.PendingNetGame->bLoadedMapSuccessfully)
        {
            // PendingNetGame의 NetDriver를 새 World로 이전하고 정리
            Context.PendingNetGame->TravelCompleted(this, Context);
            
            // PendingNetGame 제거
            Context.PendingNetGame = nullptr;
        }
    }
}
```

---

## Q) 그런데 Unreal은 왜 이런 임시객체를 만든걸까? 그냥 Connection을 다 맡겨버리면 안되나?

* 상태 관리의 분리
    * UWorld와 게임플레이 상태를 완전히 분리하여 관리
    * 연결 과정에서 발생할 수 있는 실패나 오류를 게임 월드와 독립적으로 처리
* 맵 로딩과 전환 처리
    * 새로운 맵을 로딩하는 동안 임시 상태를 관리
    * 맵 로딩이 완료되면 UWorld로 깔끔하게 전환
* 연결 프로세스의 단계별 관리
    * 핸드쉐이크, 조인 요청 등 연결 단계를 명확하게 구분하여 관리
    * 각 단계별로 필요한 처리를 독립적으로 수행
* 리소스 관리
    * 연결 과정에서 사용되는 NetDriver를 임시로 관리
    * 연결이 성공하면 UWorld로 깔끔하게 이관

```cpp
// 좋지 않은 예시 - UWorld에서 직접 처리할 경우
class UWorld {
    bool bIsConnecting;
    bool bIsLoadingMap;
    bool bIsHandshaking;
    UNetDriver* NetDriver;
    
    void ConnectToServer(const FURL& URL) {
        // 연결 중에 게임플레이도 처리해야 함
        bIsConnecting = true;
        InitNetDriver();
        BeginHandshake();
        // 맵 로딩도 처리해야 함
        LoadNewMap();
        // 게임플레이 상태와 연결 상태가 뒤섞임
    }

    void Tick() {
        if (bIsConnecting) {
            // 연결 처리
            ProcessConnection();
        }
        
        // 일반 게임플레이 틱
        ProcessGameplay();
        
        if (bIsLoadingMap) {
            // 맵 로딩 처리
            ProcessMapLoading();
        }
    }
};
```

```cpp
// 좋은 예시 - UPendingNetGame 사용
class UGameInstance {
    UWorld* CurrentWorld;
    UPendingNetGame* PendingGame;
    
    void ConnectToServer(const FURL& URL) {
        // 연결 처리를 완전히 분리
        PendingGame = NewObject<UPendingNetGame>();
        PendingGame->Initialize(URL);
        
        // 연결 성공 시 콜백
        PendingGame->OnConnectionSuccess.BindLambda([this]() {
            // 새로운 월드로 전환
            CurrentWorld = PendingGame->TransitionToWorld();
            PendingGame = nullptr;
        });
    }
    
    void Tick() {
        if (PendingGame) {
            // 연결 처리는 PendingGame에서 독립적으로
            PendingGame->Tick();
        }
        else if (CurrentWorld) {
            // 일반 게임플레이는 World에서
            CurrentWorld->Tick();
        }
    }
};

class UPendingNetGame {
    void Tick() {
        // 연결 관련 처리만 집중
        ProcessHandshake();
        ProcessMapLoading();
        NetDriver->Tick();
        
        if (bLoadedMapSuccessfully && bSuccessfullyConnected) {
            OnConnectionSuccess.Execute();
        }
    }
    
    UWorld* TransitionToWorld() {
        UWorld* NewWorld = CreateNewWorld();
        // NetDriver를 새로운 월드로 이관
        NewWorld->NetDriver = NetDriver;
        return NewWorld;
    }
};
```

---

## Tip) PlantUML Code

```
@startuml
participant "UEngine" as Engine
participant "UPendingNetGame" as PNG
participant "UNetDriver" as NetDriver
participant "Server" as Server

note over Engine: 클라이언트가 서버 접속 시도

Engine -> Engine: Browse(WorldContext, URL, Error)
activate Engine

Engine -> PNG: NewObject<UPendingNetGame>()
activate PNG

Engine -> PNG: Initialize(URL)
note right: URL에 서버 정보 저장

Engine -> PNG: InitNetDriver()
PNG -> NetDriver: NewObject<UNetDriver>()
activate NetDriver

PNG -> NetDriver: InitConnect()
NetDriver -> Server: 초기 연결 시도

note over PNG: 이 시점에서 PendingNetGame이\n서버와의 연결을 관리

Engine -> PNG: Tick(DeltaSeconds)
PNG -> NetDriver: TickDispatch()
NetDriver -> PNG: NotifyControlMessage(NMT_Welcome)
note right: 서버로부터 Welcome 메시지 수신

PNG -> PNG: bSuccessfullyConnected = true

Engine -> Engine: LoadMap(Context, URL, PendingNetGame)
note right: 서버가 지정한 맵 로드

Engine -> Engine: MovePendingLevel()
note right: PendingNetGame의 NetDriver를\n새로운 World로 이전

Engine -> PNG: LoadMapCompleted()
deactivate NetDriver

Engine -> PNG: TravelCompleted()
note right: 맵 로드 완료, PendingNetGame 제거

deactivate PNG
deactivate Engine

@enduml
```