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