---
layout: default
title: "02-03. ServerListen"
parent: "([Network] 02. ServerListen)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
/** above function but called a frame later, to stop PIE login from happening from a network callback */
// 위 함수는 네트워크 콜백에서 PIE 로그인이 발생하는 것을 방지하기 위해 한 프레임 후에 호출됩니다.
virtual void OnLoginPIEComplete_Deferred(int32 LocalUserNum, bool bWasSuccessful, FString ErrorString, FPieLoginStruct DataStruct)
{
    // create a new GameInstance for this
    // this is the main entry to create PIE Game instance in the same process with the editor executable

    // 이를 위해 새로운 GameInstance를 생성합니다.
    // 이것은 에디터 실행 파일과 동일한 프로세스에서 PIE 게임 인스턴스를 생성하는 주요 진입점입니다.
    CreateInnerProcessPIEGameInstance(PlayInEditorSessionInfo->OriginalRequestParams, DataStruct.GameInstancePIEParameters, DataStruct.PIEInstanceIndex);
}
```

```cpp
/** create an GameInstance with the given settings. a window is created if this isn't server */
UGameInstance* CreateInnerProcessPIEGameInstance(FRequestPlaySessionParams& InParams, const FGameInstancePIEParameters& InPIEParameters, int32 InPIEInstanceIndex)
{
    // ...

    // now we successfully create all necessary objects for PIE:
    // - it's time to initialize 'GameInstance' to play a game
    FGameInstancePIEResult StartResult = FGameInstancePIEResult::Success();
    {
        // 여기서 부터 ServerListen이 시작된다.
        StartResult = GameInstance->StartPlayInEditorGameInstance(NewLocalPlayer, InPIEParameters);
    }

    return GameInstance;
}
```

```cpp
/** called to actually start the game when doing play/simulate in editor */
/** 에디터에서 플레이/시뮬레이션을 할 때 실제로 게임을 시작하기 위해 호출됨 */
virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
    UEditorEngine* const EditorEngine = CastChecked<UEditorEngine>(GetEngine());
    {
        // for clients, just connect to the server
        // for networking, we add this:
        // - in networking, client-side and server(or standalone) game has different path

        // 클라이언트의 경우, 단순히 서버에 연결
        // 네트워킹을 위해 이것을 추가합니다:
        // - 네트워킹에서 클라이언트 측과 서버(또는 독립 실행형) 게임은 다른 경로를 가집니다
        if (Params.NetMode == PIE_Client)
        {
            // 이후 ClientConnect에서 다룰 것입니다
            
            // in "Networking - ServerListen", skip this part, we'll cover this in "ClientConnect"
            // "Networking - ServerListen"에서는 이 부분을 건너뛰고, "ClientConnect"에서 다룰 것입니다

            // BaseURL has (127.0.0.1:1777)
            // BaseURL은 (127.0.0.1:1777)을 가집니다
            FURL BaseURL = WorldContext->LastURL;
            FString URLString(TEXT("127.0.0.1"));

            uint16 ServerPort = 0;
            if (Params.EditorPlaySettings->GetServerPort(ServerPort))
            {
                URLString += FString::Printf(TEXT(":%hu"), ServerPort);
            }

            // before getting into UEngine::Browse, see rest of code below:
            // - we are waiting to complete connect process

            // UEngine::Browse로 들어가기 전에 아래 코드를 보세요:
            // - 연결 프로세스가 완료되기를 기다리고 있습니다
            if (EditorEngine->Browse(*WorldContext, FURL(&BaseURL, *URLString, (ETravelType)TRAVEL_Absolute), Error) == EBrowseReturnVal::Pending)
            {
                EditorEngine->TransitionType = ETransitionType::WaitingToConnect;
            }
            else
            {
                return FGameInstancePIEResult::Failure();
            }
        }
        else // (주의) PIE_ListenServer뿐만 아니라 PIE_Standalone도 이 else 문으로 흐릅니다
        {
            // we're going to be playing in the current world, get it ready for play
            // 현재 월드에서 플레이할 것이므로, 플레이를 위해 준비합니다
            UWorld* const PlayWorld = GetWorld();

            // make URL
            // URL 생성
            // in URL, it contains Map we starting on (PIEWorld)
            // URL에는 우리가 시작하는 맵(PIEWorld)이 포함되어 있습니다
            FURL URL;
            {
                //...
            }

            // we create GameMode here
            // 여기서 GameMode를 생성합니다
            PlayWorld->SetGameMode(URL);

            // create/initialize levels' actors
            // 레벨의 액터들을 생성/초기화합니다
            PlayWorld->InitializeActorsForPlay(URL);

            if (LocalPlayer)
            {
                FString Error;

                // now try to spawn APlayerController for LocalPlayer
                // 이제 LocalPlayer를 위한 APlayerController를 스폰하려고 시도합니다
                if (!LocalPlayer->SpawnPlayActor(URL.ToString(1), Error, PlayWorld))
                {
                    return FGameInstancePIEResult::Failure(FText::Format(NSLOCTEXT("UnrealEd", "Error_CouldntSpawnPlayer", "Couldn't spawn player: {0}"), FText::FromString(Error)));
                }
            }

            // if we are running listen-server, not standalone
            // - when we create PIEInstance-0, Params.NetMode is 'PIE_ListenServer'
            // 리슨 서버를 실행 중이라면 (독립 실행형이 아님)
            // - PIEInstance-0을 생성할 때, Params.NetMode는 'PIE_ListenServer'입니다
            if (Params.NetMode == PIE_ListenServer)
            {
                // 포트 추가
                // get the default server port and set it as listen port also
                // 기본 서버 포트를 가져와서 리슨 포트로도 설정합니다
                uint32 ListenPort = 0;
                uint16 ServerPort = 0;
                if (Params.EditorPlaySettings->GetServerPort(ServerPort))
                {
                    ListenPort = ServerPort;
                }

                // 리슨 서버 시작
                // EnableListenServer 다음장 참조
                EnableListenServer(true, ListenPort);
            }

            // 이제 마침내 UWorld::BeginPlay()에 도달했습니다!
            PlayWorld->BeginPlay();
        }
    }
}
```


