---
layout: default
title: "02-07. FWorldContext"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 원론적 질문, 왜 FWorldContext가 필요할까? World만으로안되나?

### 1. 관심사의 분리

```cpp
// World는 게임플레이 관련 상태만 관리
class UWorld 
{
    // 게임플레이 관련 상태들
    AGameMode* GameMode;
    AGameState* GameState;
    TArray<AActor*> Actors;
    // ...
};

// WorldContext는 엔진 레벨의 관리를 담당
struct FWorldContext 
{
    // 엔진 레벨 상태 관리
    EWorldType::Type WorldType;        // Editor/PIE/Game 구분
    TObjectPtr<UWorld> ThisCurrentWorld;
    TObjectPtr<UGameViewportClient> GameViewport;
    TArray<FNamedNetDriver> ActiveNetDrivers;
};
```

* FWorldContext의 주요 관리 목록
* 실행 모드 관리

```cpp
struct FWorldContext 
{
    // 현재 월드가 어떤 모드로 실행되고 있는지 관리
    EWorldType::Type WorldType;  // Editor, PIE, Game 중 하나
    
    // PIE 관련 설정
    int32 PIEInstance;          // PIE 인스턴스 번호
    float PIEFixedTickSeconds;  // PIE의 고정 틱 시간
};
```

* 네트워크 시스템 관리

```cpp
struct FWorldContext 
{
    // 네트워크 드라이버 관리
    TArray<FNamedNetDriver> ActiveNetDrivers;
    
    // 네트워크 게임 접속 중 상태 관리
    TObjectPtr<UPendingNetGame> PendingNetGame;
    
    // 마지막 접속 URL 기록
    FURL LastRemoteURL;
};

// 예: 네트워크 드라이버 찾기
UNetDriver* UEngine::FindNamedNetDriver(const UWorld* InWorld, FName NetDriverName)
{
    // World가 아닌 WorldContext에서 네트워크 드라이버를 관리
    const FWorldContext* Context = GetWorldContextFromWorld(InWorld);
    if (Context)
    {
        return FindNamedNetDriver_Local(Context->ActiveNetDrivers, NetDriverName);
    }
    return nullptr;
}
```

* 렌더링 시스템 연결

```cpp
struct FWorldContext 
{
    // 게임 뷰포트 관리 (렌더링, 입력 처리)
    TObjectPtr<UGameViewportClient> GameViewport;
};

// 예: 렌더링 시스템과 연결
void UEngine::Tick()
{
    for (FWorldContext& Context : WorldList)
    {
        if (Context.GameViewport)
        {
            // 월드의 렌더링을 GameViewport를 통해 처리
            Context.GameViewport->Viewport->Draw();
        }
    }
}
```

* 게임 인스턴스 관리

```cpp
struct FWorldContext 
{
    // 게임 인스턴스 참조 관리
    TObjectPtr<UGameInstance> OwningGameInstance;
};

// 예: 게임 인스턴스와 월드 연결
void FWorldContext::SetCurrentWorld(UWorld* World)
{
    UWorld* OldWorld = ThisCurrentWorld;
    ThisCurrentWorld = World;
    
    // 게임 인스턴스에 월드 변경 통지
    if (OwningGameInstance)
    {
        OwningGameInstance->OnWorldChanged(OldWorld, ThisCurrentWorld);
    }
}
```

* 멀티 월드 시스템 지원

```cpp
class UEngine 
{
    // 여러 월드의 동시 실행 관리
    TIndirectArray<FWorldContext> WorldList;
    
    // 예: PIE에서 여러 클라이언트 실행
    void StartMultiplayerPIE()
    {
        // 서버 월드 컨텍스트
        FWorldContext& ServerContext = CreateNewWorldContext(EWorldType::PIE);
        ServerContext.RunAsDedicated = true;
        
        // 클라이언트 월드 컨텍스트들
        for(int32 i = 0; i < NumClients; i++)
        {
            FWorldContext& ClientContext = CreateNewWorldContext(EWorldType::PIE);
            ClientContext.RunAsDedicated = false;
        }
    }
};
```

#### 위의 주요관리를 World에서 해주면 안되나?

> 선-결론 : World가 변경될 수 있음(새로운 지역에 간다던가) 그럴때 WorldContext를 엎는건 비효율적이다.

* 멀티플레이 PIE의 경우

```cpp
// 현재 구조
class UEngine {
    TIndirectArray<FWorldContext> WorldList;  // 여러 World 관리
};

struct FWorldContext {
    EWorldType::Type WorldType;     // Editor/PIE/Game
    int32 PIEInstance;             // PIE 인스턴스 번호
    UWorld* ThisCurrentWorld;
};

// World에 통합했다면?
    // World가 변경될때마다 엎어쳐야한다.
class UWorld {
    EWorldType::Type WorldType;
    int32 PIEInstance;
    // ... 게임플레이 관련 수많은 멤버들 ...
};
```

* 맵 전환 시나리오

```cpp
// 현재 구조
void SwitchMap() {
    FWorldContext& Context = GetWorldContext();
    UWorld* OldWorld = Context.ThisCurrentWorld;
    
    // World는 교체되어도 Context는 유지
    Context.ThisCurrentWorld = CreateNewWorld();
    DestroyWorld(OldWorld);
}

// World에 통합했다면?
void SwitchMap() {
    UWorld* OldWorld = CurrentWorld;
    UWorld* NewWorld = CreateNewWorld();
    
    // World가 파괴될 때 엔진 상태도 함께 손실
    // 새 World에 이전 상태를 복사해야 함
    NewWorld->PIEInstance = OldWorld->PIEInstance;
    NewWorld->WorldType = OldWorld->WorldType;
    // ... 기타 엔진 상태 복사 ...
    
    DestroyWorld(OldWorld);
}
```

* 메모리 및 성능 측면

```cpp
// 현재 구조
struct FWorldContext {
    // 엔진 레벨 상태 (적은 메모리)
    EWorldType::Type WorldType;
    int32 PIEInstance;
    UGameViewportClient* GameViewport;
};

class UWorld {
    // 게임플레이 상태 (큰 메모리)
    TArray<AActor*> Actors;
    ULevel* CurrentLevel;
    // ... 많은 게임플레이 데이터 ...
};

// World에 통합했다면?
class UWorld {
    // 모든 World 인스턴스가 불필요한 엔진 상태도 포함
    EWorldType::Type WorldType;
    int32 PIEInstance;
    UGameViewportClient* GameViewport;
    
    // 게임플레이 데이터
    TArray<AActor*> Actors;
    ULevel* CurrentLevel;
    // ...
};
```

---

## FWorldContext 주요 특징

1. 엔진 레벨에서 UWorld를 관리하기 위한 컨테이너
2. GameEngine에서는 보통 하나의 WorldContext만 존재
3. EditorEngine에서는:
    - 에디터 월드용 하나
    - PIE 실행 시 추가 WorldContext 생성
    - 멀티플레이 PIE 실행 시 여러 개의 WorldContext 존재 가능

```cpp
struct FWorldContext
{
    // 현재 월드의 타입 (Editor, PIE, Game 등)
    TEnumAsByte<EWorldType::Type> WorldType;
    
    // 현재 참조하고 있는 월드
    TObjectPtr<UWorld> ThisCurrentWorld;
    
    // 게임 뷰포트 클라이언트 
    TObjectPtr<UGameViewportClient> GameViewport;
    
    // PIE 인스턴스 번호 (-1이 기본값)
    int32 PIEInstance;
    
    // 활성화된 네트워크 드라이버들의 목록
    TArray<FNamedNetDriver> ActiveNetDrivers;
    
    // 임시 네트워크 게임 상태를 위한 PendingNetGame
    TObjectPtr<UPendingNetGame> PendingNetGame;
};
```

