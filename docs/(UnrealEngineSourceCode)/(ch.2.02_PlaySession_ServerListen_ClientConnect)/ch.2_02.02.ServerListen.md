---
layout: default
title: "02-02. ServerListen"
parent: "([Network] 02.PlaySession, ServerListen, ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

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
virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
    UEditorEngine* const EditorEngine = CastChecked<UEditorEngine>(GetEngine());
    {
        // ...

        // Client는 이후 강의서 다룬다.
        if (Params.NetMode == PIE_ListenServer)
        {
            // add port
            // get the default server port and set it as listen port also
            // - see *** where this ServerPort is specified in the editor binary!
            uint32 ListenPort = 0;
            uint16 ServerPort = 0;
            if (Params.EditorPlaySettings->GetServerPort(ServerPort))
            {
                ListenPort = ServerPort;
            }

            // start a listen server
            EnableListenServer(true, ListenPort);
        }

        // now finally we reach to the UWorld::BeginPlay()!
        // - after SpawnPlayerActor() is finished, we are ready to call UWorld::BeginPlay()
        PlayWorld->BeginPlay();
    
    // ...
    
}
```

```cpp
/**
* turns on/off listen server capability for a game instance
* - by default, this will set up the persistent URL state so it persists across server travels and spawn the appropriate network listener
*/
virtual bool EnableListenServer(bool bEnable, int32 PortOverride = 0)
{
    UWorld* World = GetWorld();
    ENetMode ExistingMode = World->GetNetMode();

    // our world has its NetMode as 'NM_DedicatedServer', cuz we set bRunAsDedicated 'true'
    // - client can't listen server also!
    // - I recommend you to see where NM_DedicatedServer is set
    if (ExistingMode == NM_Client)
    {
        // clients cannot change to listen
        return false;
    }

    int32 DefaultListenPort = FURL::UrlConfig.DefaultPort;
    if (bEnable)
    {
        // modify the persistent URL
        // in previous callstack, we set 'PortOverride', so it'll be overriden
        // - by adding URL as "Listen", we mark world to listen
        if (PortOverride != 0)
        {
            WorldContext->LastURL.Port = PortOverride;
        }
        WorldContext->LastURL.AddOption(TEXT("Listen"));

        // we didn't have server's world net-driver, need to create new one
        if (!World->GetNetDriver())
        {
            // this actually opens the port
            FURL ListenURL = WorldContext->LastURL;

            return World->Listen(ListenURL);
        }
        else
        {
            // already listening
            return true;
        }
    }
    else
    {
        //...

        return true;
    }
}
```

```cpp
/** start listening for connections */
bool Listen(FURL& InURL)
{
    // as you can see Listen()'s code only compiled with server-build
    // - by wrapping-up WITH_SERVER_CODE, we can hide server-side codes to client-binary!
    // - highly recommended to wrap WITH_SERVER_CODE for server-side code in your content-code, for security purpose
    // - in MMORPG, it is VERY VERY important to prevent server's binary or pdb from leaking!
    // - in dedicated server of unreal, it is totally same!
    // *** hiding server code is the first step to fight with hackers (cheating companys)
    // *** share typical MMORPG server-side anti-cheat cases
#if WITH_SERVER_CODE || 1
    // create net driver
    // - "GameNetDriver" == NAME_GameNetDriver
    if (GEngine->CreateNamedNetDriver(this, NAME_GameNetDriver, NAME_GameNetDriver))
    {
        // we should find NetDriver(IpNetDriver)
        NetDriver = GEngine->FindNamedNetDriver(this, NAME_GameNetDriver);

        // haker:
        // *** UNetDriver::SetWorld is important!
        NetDriver->SetWorld(this);
    }

    if (NetDriver == nullptr)
    {
        // netdriver creation is failed
        return false;
    }

    FString Error;
    if (!NetDriver->InitListen(this, InURL, false, Error))
    {
        return false;
    }

    return true;
#endif
}
```