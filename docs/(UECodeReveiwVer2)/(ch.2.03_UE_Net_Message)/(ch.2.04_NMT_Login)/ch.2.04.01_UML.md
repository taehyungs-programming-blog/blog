---
layout: default
title: "[MSG] NMT_Login"
parent: "([Network] 03. UE Net의 주요 메시지 정리)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/ue_network_2025_2_4_1.png"/>
</p>

```cpp

// 1. UWorld에서 NMT_Login 메시지 처리
void UWorld::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch)
{
    case NMT_Login:
    {
        // 로그인 메시지 수신
        FUniqueNetIdRepl UniqueIdRepl;
        FString OnlinePlatformName;
        FString RequestURL;
        
        bool bReceived = FNetControlMessage<NMT_Login>::Receive(
            Bunch, 
            Connection->ClientResponse, 
            RequestURL, 
            UniqueIdRepl, 
            OnlinePlatformName
        );

        if (bReceived)
        {
            // Connection에 플레이어 정보 설정
            Connection->PlayerId = UniqueIdRepl;
            Connection->SetPlayerOnlinePlatformName(FName(*OnlinePlatformName));

            // GameMode의 PreLogin 처리
            AGameModeBase* GameMode = GetAuthGameMode();
            if (GameMode)
            {
                GameMode->PreLoginAsync(
                    *RequestURL,
                    Connection->LowLevelGetRemoteAddress(),
                    Connection->PlayerId,
                    AGameModeBase::FOnPreLoginCompleteDelegate::CreateUObject(
                        this, 
                        &UWorld::PreLoginComplete, 
                        TWeakObjectPtr<UNetConnection>(Connection)
                    )
                );
            }
        }
    }
    break;
}

// 2. GameMode의 PreLogin 처리
void AGameModeBase::PreLoginAsync(
    const FString& Options, 
    const FString& Address, 
    const FUniqueNetIdRepl& UniqueId,
    const FOnPreLoginCompleteDelegate& Delegate)
{
    // GameSession을 통해 플레이어 접속 가능 여부 확인
    FString ErrorMessage;
    AGameSession* const GameSession = GameState->GameSession;
    if (GameSession)
    {
        // MaxPlayers 등 체크
        ErrorMessage = GameSession->PreLogin(Options, Address, UniqueId);
    }

    // 결과 콜백
    Delegate.ExecuteIfBound(ErrorMessage);
}

// 3. GameSession의 PreLogin 체크
FString AGameSession::PreLogin(
    const FString& Options, 
    const FString& Address, 
    const FUniqueNetIdRepl& UniqueId)
{
    // MaxPlayers 체크
    if (MaxPlayers > 0)
    {
        int32 CurrentConnections = GetNumberOfPlayersOnServer();
        if (CurrentConnections >= MaxPlayers)
        {
            return TEXT("Server is full.");
        }
    }
    return TEXT("");  // 빈 문자열은 성공을 의미
}

// 4. World의 PreLogin 완료 처리
void UWorld::PreLoginComplete(
    const FString& ErrorMsg, 
    TWeakObjectPtr<UNetConnection> WeakConnection)
{
    UNetConnection* Connection = WeakConnection.Get();
    if (!PreLoginCheckError(Connection, ErrorMsg))
    {
        return;  // 에러가 있으면 종료
    }

    // 성공시 Welcome 메시지 전송
    WelcomePlayer(Connection);
}

// 5. Welcome 메시지 전송
void UWorld::WelcomePlayer(UNetConnection* Connection)
{
    // 현재 레벨 정보 설정
    FString LevelName = CurrentLevel->GetOutermost()->GetName();
    Connection->SetClientWorldPackageName(
        CurrentLevel->GetOutermost()->GetFName()
    );

    // GameMode 정보 설정
    FString GameName;
    if (AuthorityGameMode != NULL)
    {
        GameName = AuthorityGameMode->GetClass()->GetPathName();
    }

    // Welcome 메시지 전송
    FNetControlMessage<NMT_Welcome>::Send(
        Connection, 
        LevelName,
        GameName, 
        FString()  // RedirectURL
    );

    Connection->FlushNet();
    Connection->QueuedBits = 0;
    Connection->SetClientLoginState(EClientLoginState::Welcomed);
}

```

---

## Tip) PlantUML

```
@startuml
participant Client
participant "Server\nUWorld" as World
participant UNetConnection
participant AGameModeBase
participant AGameSession

Client -> World: NMT_Login
activate World

World -> UNetConnection: FNetControlMessage<NMT_Login>::Receive()
activate UNetConnection
UNetConnection --> World: {ClientResponse, RequestURL, UniqueIdRepl, OnlinePlatformName}
deactivate UNetConnection

World -> UNetConnection: Set PlayerId & OnlinePlatformName
activate UNetConnection
deactivate UNetConnection

World -> AGameModeBase: PreLoginAsync()
activate AGameModeBase

AGameModeBase -> AGameSession: PreLogin()
activate AGameSession

alt MaxPlayers Check
    AGameSession -> AGameSession: Check MaxPlayers
    alt MaxPlayers Exceeded
        AGameSession --> AGameModeBase: Error
        AGameModeBase --> World: Error
        World --> Client: Disconnect
    else MaxPlayers Available
        AGameSession --> AGameModeBase: Success
        AGameModeBase --> World: Success
        World -> World: WelcomePlayer()
        World --> Client: NMT_Welcome
    end
end

deactivate AGameSession
deactivate AGameModeBase
deactivate World

@enduml
```