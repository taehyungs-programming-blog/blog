---
layout: default
title: "02-01. PlaySession"
parent: "([Network] 02.PlaySession, ServerListen, ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* `RequestPlaySession`는 어디서 호출되었나? 궁금하겠지만 큰 의미가 없는게 PIE에서 Play 누르면 거의 바로 호출이 됨. 
* 즉 `RequestPlaySession`이전은 크게 볼 의미는 없다.

* 잠깐 복습, **Unreal CDO(Class Default Object)**
    * Unreal Engine에서 **CDO(Class Default Object)**는 클래스의 기본 객체를 의미합니다. 
    * CDO는 특정 클래스의 초기 기본값들을 저장하고 있으며, 새로운 인스턴스가 생성될 때 이 CDO를 기반으로 초기화됩니다. 
    * 이는 객체 생성 시 일관된 초기 상태를 보장하며, 메모리 사용과 성능 측면에서 효율성을 제공합니다.
    * **주요 특징:**
        * 초기값 저장: CDO는 클래스의 모든 프로퍼티에 대한 기본값을 저장합니다. 이를 통해 객체를 생성할 때마다 프로퍼티를 개별적으로 설정할 필요가 없습니다.
        * 메모리 효율성: 동일한 값들은 메모리에 한 번만 저장되며, 각 인스턴스는 이 값을 참조합니다.
        * Reflection 시스템과의 연동: CDO는 Unreal Engine의 Reflection 시스템과 밀접하게 연관되어 있어, 에디터 내에서의 프로퍼티 편집, 시리얼라이제이션, 복제 등 다양한 기능에 활용됩니다.
        * Garbage Collection: CDO는 Garbage Collector가 추적하는 루트 객체 중 하나로, 메모리 관리에 중요한 역할을 합니다.

```cpp
/** request a play session (PIE, Play-in-New-Process, Launcher) with the configured parameters; see FRequestPlaySessionParams for more details */
// when you press the PIE button, this method is called to queue the PIE request
void RequestPlaySession(const FRequestPlaySessionParams& InParams)
{
    // store our request to be operated on next Tick
    // see PlaySessionRequest member-variable briefly ***:
    // - it holds only one request, multiple requests are converged into one, the latest one
    // - see FRequestPlaySessionParams briefly ***
    PlaySessionRequest = InParams;

    // usually this EditorPlaySettings are nullptr, so use the CDO of ULevelEditorPlaySettings:
    //                                                                                                            
    //                                                                                                            
    //                                                                                                            
    //              C++ Compiled Version     │    Serializable C++                                                
    //                                       │                                                                    
    //                                       │                                                                    
    //                                       │                                                                    
    //         class (C++)◄──────────────────┼───────UClass(Unreal)                                               
    //          │                            │        │                                                           
    //          └──Initialization-list       │        └──CDO (Class Default Object)                               
    //                    ▲                  │                    │                                               
    //                    └──────────────────┼────────────────────┘                                               
    //                                       │     *** CDO is a format of serializable c++ initialization-list    
    //                                       │                            ─────────────────────────────────────   
    //                                       │                                                                    
    //                                                                                                            
    if (!PlaySessionRequest->EditorPlaySettings)
    {
        // CDO를 통해 만들고
        PlaySessionRequest->EditorPlaySettings = GetMutableDefault<ULevelEditorPlaySettings>();
    }

    // now we duplicate their EditorPlaySettings so that we can mutate it as part of startup to help rule out invalid configuration combinations
    // we cannot use the CDO directly, so duplicate the CDO and use the duplicated instance instead:
    // - FObjectDuplicationParameters + StaticDuplicateObjectEx = duplication of UObject

    // CDO를 수정할순 없으니 복사해서 만든다 생각하자.
    FObjectDuplicationParameters DuplicationParams(PlaySessionRequest->EditorPlaySettings, GetTransientPackage());
    PlaySessionRequest->EditorPlaySettings = CastChecked<ULevelEditorPlaySettings>(StaticDuplicateObjectEx(DuplicationParams));
}
```


```cpp
// UEditorEngine
virtual void Tick(float DeltaSeconds, bool bIdleMode) override
{
    // ...

    // kick off a play session request if one was queued up during the last frame
    // - RequestPlaySession() sets 'PlaySessionRequest'
    // when play-session-request is set(added), it processes in the UEditorEngine::Tick
    if (PlaySessionRequest.IsSet())
    {
        // PlaySessionRequest가 세팅되었다면 여기로 들어오게 된다.
        StartQueuedPlaySessionRequest();
    }

    // ...
```

```cpp
void StartQueuedPlaySessionRequest()
{
    // - before see StartQueuedPlaySessionRequestImpl, see the statement, "PlaySessionRequest.Reset()"
    StartQueuedPlaySessionRequestImpl();

    // ensure the request is always reset after an attempt (which may fail), so that we don't get stuck in an infinite loop of start attempts
    // to accept another session request, it is natural to clear up right?

    // 여기서 Reset을 해버리는걸로 봐선... 위에서 뭔가 다 처리를 하는가 보군?
    PlaySessionRequest.Reset();
}
```

```cpp
/** start the queued PlaySessionRequest; after this is called the queued play session request will be cleared */
virtual void StartQueuedPlaySessionRequestImpl()
{
    const ULevelEditorPlaySettings* EditorPlaySettings = PlaySessionRequest->EditorPlaySettings;
    check(EditorPlaySettings);

    PlayInEditorSessionInfo = FPlayInEditorSessionInfo();

    // we preserve PlaySessionRequest in PlayInEditorSessionInfo
    PlayInEditorSessionInfo->OriginalRequestParams = PlaySessionRequest.GetValue();

    // we'll branch primarily based on the Session Destination, because it affects which settings we apply and how
    // what we are interested in is PIE, whose SessionDestination is "InProcess"
    switch (PlaySessionRequest->SessionDestination)
    {
    case EPlaySessionDestinationType::InProcess:
        // create one-or-more PIE/SIE sessions inside of the current process
        // now we start to PIE!
        StartPlayInEditorSession(PlaySessionRequest.GetValue());
        break;
    //...
    }
}
```

```cpp
/** start a PIE session with the given parameters; called by StartQueuedPlaySessionRequestImpl based on request settings */
virtual void StartPlayInEditorSession(FRequestPlaySessionParams& InRequestParams)
{
    // make sure there's no outstanding load requests
        // World를 새로 생성해야하기에 기존의 AsyncLoading은 모두 정리하자.
    FlushAsyncLoading();

    // gameplay relies on asset registry to be fully constructed, wait for completion before starting PIE
        // Asset의 사용을 위해 Asset이 모두 Cached될때까지 기다려 달라!
    IAssetRegistry::GetChecked().WaitForCompletion();

    // from here, make sure all loading requests are finished(FlushAsyncLoading()) and AssetRegistry's process is finished (make asset path available!)

    //...

    ULevelEditorPlaySettings* EditorPlaySettings = InRequestParams.EditorPlaySettings;
    check(EditorPlaySettings);

    // now that we've gotten all of the editor house-keeping out of the way we can finally start creating world instances and multi-player clients
    // as comment saids, we are starting to run gameplay or multi-player clients (including server)
    {
        // first, we handle starting a dedicated server, this can exist as either a separate process, or as an internal world
        // bUserWantsSingleProcess is 'true':
        // - if we consider that the PIE is running in one(same) process, it is natrual to get it as 'true'
        bool bUserWantsSingleProcess;
        InRequestParams.EditorPlaySettings->GetRunUnderOneProcess(bUserWantsSingleProcess);

        // in networking, we get 'NetMode' as PIE_Client:
        // - this property reflects PIE settings, see the EDITOR build
        EPlayNetMode NetMode;
        InRequestParams.EditorPlaySettings->GetPlayNetMode(NetMode);

        // standalone requires no server, and ListenServer doesn't require a separate server
        // PIE_Client means networking game, so we needs a server!
        const bool bNetModeRequiresSeparateServer = NetMode == EPlayNetMode::PIE_Client;
        const bool bNeedsServer = bNetModeRequiresSeparateServer;

        // if they require a separate server we'll give the EditorEngine a chance to handle any additional prep-work
        if (bNeedsServer)
        {
            // if they don't want single process we launch the server as a separate process
            // if they do want single process, it will get handled below as part of client startup

            // in usual case, the PIE's server is running in same process (as we saw the above, bUseWantsSingleProcess is 'true')
            // - note that when debugging the PIE, it is more convenient for server and clients reside in the same process!
            if (!bUseWantsSingleProcess)
            {
                const bool bIsDedicatedServer = true;
                const bool bIsHost = true;
                const int32 InstanceIndex = 0;
                LaunchNewProcess(InRequestParams, InstanceIndex, EPlayNetMode::PIE_ListenServer, bIsDedicatedServer);
                PlayInEditorSessionInfo->bServerWasLaunched = true;
            }
        }

        // now that the dedicated server was (optionally) started, we'll start as many requested clients as we can
        // - because the user indicated they wanted PIE/PINW we'll put the first client in the editor repecting that setting
        // - any additional clients will either be in-process new windows, or separate processes based on settings

        // see how the PIE settings are related to this variable, NumClients
        int32 NumClients;
        InRequestParams.EditorPlaySettings->GetPlayNumberOfClients(NumClients);

        // if the have a net mode that requires a server but they didn't create (or couldn't create due to single-process limitations) a dedicated one
        // , then we launch an extra world context acting as a server in-process

        // when you run a server in separate process, 'bServerWasLaunched' becomes 'true', otherwise it works as we expected
        // - if we set NumClients as 2, total instance running is 3!
        const bool bRequiresExtraListenServer = bNeedsServer && !PlayInEditorSessionInfo->bServerWasLaunched;
        int32 NumRequestedInstances = FMath::Max(NumClients, 1);
        if (bRequiresExtraListenServer)
        {
            NumRequestedInstances++;
        }

        // now we iterates all requested instances and create new instance
        for (int32 InstanceIndex = 0; InstanceIndex < NumRequestedInstances; InstanceIndex++)
        {
            // if they are running single-process and they need a server, the first instance will be the server
            // when you set the server running in same process, InstanceIndex 0 is for the server instance!
            const bool bClientIsServer = (InstanceIndex == 0) && (NetMode == EPlayNetMode::PIE_ListenServer || bRequiresExtraListenServer);

            // if they're the server, we want to override them to be a ListenServer:
            // - this will get ignored if they're secretly a dedicated server so it's okay
            EPlayNetMode LocalNetMode = NetMode;
            if (bClientIsServer)
            {
                // only index==0 is set as PIE_ListenServer
                LocalNetMode = EPlayNetMode::PIE_ListenServer;
            }

            // 'bRunAsDedicated' becomes 'true' only for index-0 in networking games
            bool bRunAsDedicated = bClientIsServer && bRequiresExtraListenServer;

            // create the instance:
            // - this can end up creating separate processes if needed based on settings
            // - this code is separated out of here, so it can be re-used by the Late Join flow.
            // 
            // - before see CreateNewPlayInEditorInstance, see the below code first!
            CreateNewPlayInEditorInstance(InRequestParams, bRunAsDedicated, LocalNetMode);

            // the first time when when we reach to this lines of the code is index-0, dedicated server instance
            // - PIEServerWorld is ready and also NetDriver is instantiated in the above CreateNewPlayInEditorInstances
            // - after index-0, the rest of indices are about 'Clients':
            //   - the clients need to connect to the server, to do that, they need to know server's port number
            //   - so, we use EditorPlaySettings to notify server-ports to the clients
            if (bClientsIsServer)
            {
                // grab new created PIE server instance and set PlaySettings to Server's actual Port so Clients Connect To Correct Server
                const FWorldContext* const PIEServerWorldContext = GetWorldContextFromPIEInstance(PlayInEditorSessionInfo->PIEInstanceCount - 1);
                const UWorld* const PIEServerWorld = PIEServerWorldContext->World();
                if (PIEServerWorld)
                {
                    UNetDriver* const NetDriver = PIEServerWorld->GetNetDriver();
                    if (NetDriver)
                    {
                        // here! we cache server port here!
                        // - to get server port number, we use GetLocalAddr()->GetPort()!
                        EditorPlaySettings->SetServerPort(NetDriver->GetLocalAddr()->GetPort());
                    }
                }
            }
        }

        //...
    }

    //...
}
```

```cpp
/**
* creates a new PIE instance (which may be in a new process if not running under one process)
* - this reads the current session state to start the next instance needed
*/
virtual void CreateNewPlayInEditorInstance(FRequestPlaySessionParams &InRequestParams, const bool bInDedicatedInstance, const EPlayNetMode InNetMode)
{
// I re-order the codes to make it more readable
{
    // create a world context for our client
    // I recommend to see CreateNewWorldContext for you who has/had taken foundation course (part 1)
    FWorldContext& PieWorldContext = CreateNewWorldContext(EWorldType::PIE);

    // unique PIE instance id is assigned by PlayInEditorSessionInfo's PIEInstanceCount
    // if we are running 2-Clients:
    // 1. server instance id == 0
    // 2. client0 instance id == 1
    // 3. client1 instance id == 2
    PieWorldContext.PIEInstance = PlayInEditorSessionInfo->PIEInstanceCount++;

    // note that 'bInDedicatedInstances' are passed as 'true' only for instance's index is '0' in networking game
    FGameInstancePIEParameters GameInstancePIEParameters;
    GameInstancePIEParameters.bRunAsDedicated = bInDedicatedInstance;
    GameInstancePIEParameters.NetMode = InNetMode;

    // see FPieLoginStruct briefly:
    // - PIELoginInfo is the 'ONE' parameter to convey the data needed for 'OnLoginPIEComplete_Deferred'
    FPieLoginStruct PIELoginInfo;
    PIELoginInfo.GameInstancePIEParameters = GameInstancePIEParameters;
    PIELoginInfo.WorldContextHandle = PieWorldContext.ContextHandle;
    PIELoginInfo.PIEInstanceIndex = PieWorldContext.PIEInstance;

    // see OnLoginPIEComplete_Deferred briefly
    OnLoginPIEComplete_Deferred(0, true, FString(), PIELoginInfo);
}
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