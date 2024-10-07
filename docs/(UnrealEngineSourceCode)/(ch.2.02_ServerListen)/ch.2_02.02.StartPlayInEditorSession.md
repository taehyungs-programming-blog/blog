---
layout: default
title: "02-02. StartPlayInEditorSession"
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
/** start a PIE session with the given parameters; called by StartQueuedPlaySessionRequestImpl based on request settings */
// 주어진 매개변수로 PIE 세션을 시작합니다; StartQueuedPlaySessionRequestImpl에 의해 요청 설정에 기반하여 호출됩니다
virtual void StartPlayInEditorSession(FRequestPlaySessionParams& InRequestParams)
{
    /*
    * 사실 여기 설명 대부분이 PIE로 실행시 
        * Server를 별도 Process로 띄울지, Client와 함께 띄울지
        * Client의 개수를 몇개로 할지
    */

    // make sure there's no outstanding load requests
    // 미처리된 로드 요청이 없는지 확인합니다
    FlushAsyncLoading();

    // gameplay relies on asset registry to be fully constructed, wait for completion before starting PIE
    // 게임플레이는 에셋 레지스트리가 완전히 구성되어 있어야 하므로, PIE를 시작하기 전에 완료를 기다립니다
    IAssetRegistry::GetChecked().WaitForCompletion();

    // from here, make sure all loading requests are finished(FlushAsyncLoading()) and AssetRegistry's process is finished (make asset path available!)
    // 여기서부터 모든 로딩 요청이 완료되었는지(FlushAsyncLoading()) 그리고 AssetRegistry 프로세스가 완료되었는지(에셋 경로를 사용할 수 있게) 확인합니다!

    //...

    ULevelEditorPlaySettings* EditorPlaySettings = InRequestParams.EditorPlaySettings;
    check(EditorPlaySettings);

    // now that we've gotten all of the editor house-keeping out of the way we can finally start creating world instances and multi-player clients
    // as comment saids, we are starting to run gameplay or multi-player clients (including server)
    // 이제 에디터의 모든 정리 작업이 끝났으므로 월드 인스턴스와 멀티플레이어 클라이언트를 생성하기 시작할 수 있습니다
    // 주석에서 말한 대로, 게임플레이나 멀티플레이어 클라이언트(서버 포함)를 실행하기 시작합니다
    {
        // first, we handle starting a dedicated server, this can exist as either a separate process, or as an internal world
        // bUserWantsSingleProcess is 'true':
        // - if we consider that the PIE is running in one(same) process, it is natrual to get it as 'true'
        // 먼저, 전용 서버 시작을 처리합니다. 이는 별도의 프로세스로 존재하거나 내부 월드로 존재할 수 있습니다
        // bUserWantsSingleProcess는 'true'입니다:
        // - PIE가 하나의(동일한) 프로세스에서 실행된다고 고려하면, 이것이 'true'가 되는 것이 자연스럽습니다
            // bUserWantsSingleProcess가 true일 경우 server와 client가 하나의 process임을 의미합니다.
        bool bUserWantsSingleProcess;
        InRequestParams.EditorPlaySettings->GetRunUnderOneProcess(bUserWantsSingleProcess);

        // in networking, we get 'NetMode' as PIE_Client:
        // - this property reflects PIE settings, see the EDITOR build
        // 네트워킹에서는 'NetMode'를 PIE_Client로 얻습니다:
        // - 이 속성은 PIE 설정을 반영합니다. EDITOR 빌드를 참조하세요
        EPlayNetMode NetMode;
        InRequestParams.EditorPlaySettings->GetPlayNetMode(NetMode);

        // standalone requires no server, and ListenServer doesn't require a separate server
        // PIE_Client means networking game, so we needs a server!
        // 독립 실행형은 서버가 필요 없고, ListenServer는 별도의 서버가 필요 없습니다
        // PIE_Client는 네트워킹 게임을 의미하므로, 서버가 필요합니다!
        const bool bNetModeRequiresSeparateServer = NetMode == EPlayNetMode::PIE_Client;
        const bool bNeedsServer = bNetModeRequiresSeparateServer;

        // if they require a separate server we'll give the EditorEngine a chance to handle any additional prep-work
        // 별도의 서버가 필요한 경우 EditorEngine에게 추가 준비 작업을 처리할 기회를 줍니다
        if (bNeedsServer)
        {
            // if they don't want single process we launch the server as a separate process
            // if they do want single process, it will get handled below as part of client startup
            // in usual case, the PIE's server is running in same process (as we saw the above, bUseWantsSingleProcess is 'true')
            // - note that when debugging the PIE, it is more convenient for server and clients reside in the same process!
            // 단일 프로세스를 원하지 않는 경우 서버를 별도의 프로세스로 실행합니다
            // 단일 프로세스를 원하는 경우, 아래에서 클라이언트 시작의 일부로 처리됩니다
            // 일반적인 경우, PIE의 서버는 동일한 프로세스에서 실행됩니다 (위에서 본 것처럼 bUseWantsSingleProcess가 'true'입니다)
            // - PIE를 디버깅할 때는 서버와 클라이언트가 동일한 프로세스에 있는 것이 더 편리합니다!
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
        // 이제 전용 서버가 (선택적으로) 시작되었으므로, 요청된 만큼의 클라이언트를 시작합니다
        // - 사용자가 PIE/PINW를 원한다고 표시했으므로 첫 번째 클라이언트를 해당 설정을 존중하여 에디터에 배치합니다
        // - 추가 클라이언트는 설정에 따라 프로세스 내 새 창이나 별도의 프로세스가 됩니다

        // see how the PIE settings are related to this variable, NumClients
        // PIE 설정이 이 변수 NumClients와 어떻게 관련되는지 봅니다
        int32 NumClients;
        InRequestParams.EditorPlaySettings->GetPlayNumberOfClients(NumClients);

        // if the have a net mode that requires a server but they didn't create (or couldn't create due to single-process limitations) a dedicated one
        // , then we launch an extra world context acting as a server in-process
        // 서버가 필요한 넷 모드를 가지고 있지만 전용 서버를 생성하지 않았거나 (또는 단일 프로세스 제한으로 인해 생성할 수 없는 경우)
        // 프로세스 내에서 서버 역할을 하는 추가 월드 컨텍스트를 실행합니다

        // when you run a server in separate process, 'bServerWasLaunched' becomes 'true', otherwise it works as we expected
        // - if we set NumClients as 2, total instance running is 3!
        // 서버를 별도의 프로세스에서 실행할 때 'bServerWasLaunched'는 'true'가 되고, 그렇지 않으면 예상대로 작동합니다
        // - NumClients를 2로 설정하면 실행 중인 총 인스턴스는 3개입니다!
        const bool bRequiresExtraListenServer = bNeedsServer && !PlayInEditorSessionInfo->bServerWasLaunched;
        int32 NumRequestedInstances = FMath::Max(NumClients, 1);
        if (bRequiresExtraListenServer)
        {
            NumRequestedInstances++;
        }

        // now we iterates all requested instances and create new instance
        // 이제 모든 요청된 인스턴스를 반복하고 새 인스턴스를 생성합니다
        for (int32 InstanceIndex = 0; InstanceIndex < NumRequestedInstances; InstanceIndex++)
        {
            // if they are running single-process and they need a server, the first instance will be the server
            // when you set the server running in same process, InstanceIndex 0 is for the server instance!
            // 단일 프로세스로 실행 중이고 서버가 필요한 경우, 첫 번째 인스턴스가 서버가 됩니다
            // 서버를 동일한 프로세스에서 실행하도록 설정하면 InstanceIndex 0이 서버 인스턴스입니다!
            const bool bClientIsServer = (InstanceIndex == 0) && (NetMode == EPlayNetMode::PIE_ListenServer || bRequiresExtraListenServer);

            // if they're the server, we want to override them to be a ListenServer:
            // - this will get ignored if they're secretly a dedicated server so it's okay
            // 서버인 경우 ListenServer로 오버라이드하려고 합니다:
            // - 비밀리에 전용 서버인 경우 이는 무시되므로 괜찮습니다
            EPlayNetMode LocalNetMode = NetMode;
            if (bClientIsServer)
            {
                // only index==0 is set as PIE_ListenServer
                // index==0만 PIE_ListenServer로 설정됩니다
                LocalNetMode = EPlayNetMode::PIE_ListenServer;
            }

            // 'bRunAsDedicated' becomes 'true' only for index-0 in networking games
            // 'bRunAsDedicated'는 네트워킹 게임에서 index-0인 경우에만 'true'가 됩니다
            bool bRunAsDedicated = bClientIsServer && bRequiresExtraListenServer;

            // create the instance:
            // - this can end up creating separate processes if needed based on settings
            // - this code is separated out of here, so it can be re-used by the Late Join flow.
            
            // 인스턴스를 생성합니다:
            // - 설정에 따라 필요한 경우 별도의 프로세스를 생성할 수 있습니다
            // - 이 코드는 여기서 분리되어 Late Join 흐름에서 재사용될 수 있습니다.
            // CreateNewPlayInEditorInstance는 아래 참조 (1)
            CreateNewPlayInEditorInstance(InRequestParams, bRunAsDedicated, LocalNetMode);

            // the first time when when we reach to this lines of the code is index-0, dedicated server instance
            // - PIEServerWorld is ready and also NetDriver is instantiated in the above CreateNewPlayInEditorInstances
            // - after index-0, the rest of indices are about 'Clients':
            //   - the clients need to connect to the server, to do that, they need to know server's port number
            //   - so, we use EditorPlaySettings to notify server-ports to the clients

            // 이 코드 라인에 처음 도달할 때는 index-0, 전용 서버 인스턴스입니다
            // - PIEServerWorld가 준비되고 위의 CreateNewPlayInEditorInstances에서 NetDriver도 인스턴스화됩니다
            // - index-0 이후, 나머지 인덱스들은 '클라이언트'에 관한 것입니다:
            //   - 클라이언트는 서버에 연결해야 하며, 이를 위해 서버의 포트 번호를 알아야 합니다
            //   - 따라서 EditorPlaySettings를 사용하여 클라이언트에게 서버 포트를 알립니다
            if (bClientsIsServer)
            {
                // grab new created PIE server instance and set PlaySettings to Server's actual Port so Clients Connect To Correct Server
                // 새로 생성된 PIE 서버 인스턴스를 가져와서 PlaySettings를 서버의 실제 포트로 설정하여 클라이언트가 올바른 서버에 연결하도록 합니다
                const FWorldContext* const PIEServerWorldContext = GetWorldContextFromPIEInstance(PlayInEditorSessionInfo->PIEInstanceCount - 1);
                const UWorld* const PIEServerWorld = PIEServerWorldContext->World();
                if (PIEServerWorld)
                {
                    UNetDriver* const NetDriver = PIEServerWorld->GetNetDriver();
                    if (NetDriver)
                    {
                        // here! we cache server port here!
                        // - to get server port number, we use GetLocalAddr()->GetPort()!
                        // 여기서 서버 포트를 캐시합니다!
                        // - 서버 포트 번호를 얻기 위해 GetLocalAddr()->GetPort()를 사용합니다!
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

---

## (1) CreateNewPlayInEditorInstance

```cpp
/**
    * creates a new PIE instance (which may be in a new process if not running under one process)
    * - this reads the current session state to start the next instance needed
    */
// 새로운 PIE 인스턴스를 생성합니다 (단일 프로세스에서 실행되지 않는 경우 새 프로세스에서 생성될 수 있음)
// - 이는 다음에 필요한 인스턴스를 시작하기 위해 현재 세션 상태를 읽습니다
virtual void CreateNewPlayInEditorInstance(FRequestPlaySessionParams &InRequestParams, const bool bInDedicatedInstance, const EPlayNetMode InNetMode)
{
    // (참고) 코드를 더 읽기 쉽게 재정렬했습니다
    {
        // create a world context for our client
        // 클라이언트를 위한 월드 컨텍스트를 생성합니다
            // (참고) CreateNewWorldContext를 살펴보면 이해가 편합니다.
        FWorldContext& PieWorldContext = CreateNewWorldContext(EWorldType::PIE);

        // unique PIE instance id is assigned by PlayInEditorSessionInfo's PIEInstanceCount
        // if we are running 2-Clients:
        // 1. server instance id == 0
        // 2. client0 instance id == 1
        // 3. client1 instance id == 2

        // 고유한 PIE 인스턴스 ID는 PlayInEditorSessionInfo의 PIEInstanceCount에 의해 할당됩니다
        // 2개의 클라이언트를 실행하는 경우:
        // 1. 서버 인스턴스 ID == 0
        // 2. 클라이언트0 인스턴스 ID == 1
        // 3. 클라이언트1 인스턴스 ID == 2
        PieWorldContext.PIEInstance = PlayInEditorSessionInfo->PIEInstanceCount++;

        // note that 'bInDedicatedInstances' are passed as 'true' only for instance's index is '0' in networking game
        // 'bInDedicatedInstances'는 네트워킹 게임에서 인스턴스의 인덱스가 '0'인 경우에만 'true'로 전달됩니다
        FGameInstancePIEParameters GameInstancePIEParameters;
        GameInstancePIEParameters.bRunAsDedicated = bInDedicatedInstance;
        GameInstancePIEParameters.NetMode = InNetMode;

        // - PIELoginInfo is the 'ONE' parameter to convey the data needed for 'OnLoginPIEComplete_Deferred'
        // - PIELoginInfo는 'OnLoginPIEComplete_Deferred'에 필요한 데이터를 전달하는 '하나의' 매개변수입니다
        FPieLoginStruct PIELoginInfo;
        PIELoginInfo.GameInstancePIEParameters = GameInstancePIEParameters;
        PIELoginInfo.WorldContextHandle = PieWorldContext.ContextHandle;
        PIELoginInfo.PIEInstanceIndex = PieWorldContext.PIEInstance;

        OnLoginPIEComplete_Deferred(0, true, FString(), PIELoginInfo);
    }
}
```

```cpp
/** above function but called a frame later, to stop PIE login from happening from a network callback */
virtual void OnLoginPIEComplete_Deferred(int32 LocalUserNum, bool bWasSuccessful, FString ErrorString, FPieLoginStruct DataStruct)
{
    // create a new GameInstance for this
    // this is the main entry to create PIE Game instance in the same process with the editor executable
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