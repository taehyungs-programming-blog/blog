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

    // Unreal은 보통 Client와 Server가 같이 작업되는 형태가 많다.
    // WITH_SERVER_CODE을 둠으로써 Client 바이너리에 Server코드가 같이 안딸려가는 효과가 있음!
#if WITH_SERVER_CODE
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

```cpp
/** create a UNetDriver and associates a name with it */
bool CreateNamedNetDriver(UWorld *InWorld, FName NetDriverName, FName NetDriverDefinition)
{
    // - note that second parameter is 'NetDriverName' and third parameter is 'NetDriverDefinition', which we'll cover it soon
    return CreateNamedNetDriver_Local(this, GetWorldContextFromWorldChecked(InWorld), NetDriverName, NetDriverDefinition);
}
```

```cpp
bool CreateNamedNetDriver_Local(UEngine* Engine, FWorldContext& Context, FName NetDriverName, FName NetDriverDefinition)
{
    // we first try to find any existing net-driver
    // *** see FWorldContext::ActiveNetDrivers briefly
    // - see FindNamedNetDriver_Local briefly
    //   - NetDriverName is "GameNetDriver"
    UNetDriver* NetDriver = FindNamedNetDriver_Local(Context.ActiveNetDrivers, NetDriverName);

    // in our case, our NetDriver is nullptr, so need to create new one
    if (NetDriver == nullptr)
    {
        NetDriver = CreateNetDriver_Local(Engine, Context, NetDriverDefinition, NetDriverName);
        if (NetDriver)
        {
            return true;
        }
    }
    return false;
}
```

```cpp
/**
* associate a world with this net driver
* disassociation any previous world first
*/
// UNetDriver::SetWorld() is one of important methods you need to know:
// - this method links tick-events of NetDriver to the world

// 결국 NetDriver도 World에 기생하여 Tick Event를 수신해야 한다.
virtual void SetWorld(UWorld* InWorld)
{
    // first, unlink all previously bound netdriver's tick events
    if (World)
    {
        // remove old world association
        UnregisterTickEvents(World);
        World = nullptr;
        WorldPackage = nullptr;
        Notify = nullptr;
    }

    // let's see what kind of tick-events need to be bound to the world's tick
    // - see RegisterTickEvents()
    if (InWorld)
    {
        // setup new world association
        World = InWorld;
        WorldPackage = InWorld->GetOutermost();
        Notify = InWorld;
        RegisterTickEvents(InWorld);
    }


    // World-NetDriver 관계를 좀 더 살펴보자면 ..
        // 궁금한점? ActiveNetDrivers를 배열로 관리할 필요가 있나?
        // -> 있다, DemoNetDriver가 필요하기 때문,
        // 그럼 Demo는 뭐하는 애지? -> 가령, replay를 할때 화면을 저장해 두고 할까? -> Nope! 패킷을 뒀다가 다시 실행하는 형태이다. 이때 사용된다.
    // try to understand the relationship between World and NetDriver:
    //
    //    WorldContext: FWorldContext                                                      
    //      │                                                                              
    //      ├───World: UWorld                                                              
    //      │                                                                              
    //      ├───GameViewport: UGameViewportClient                                          
    //      │                                                                              
    //      └───ActiveNetDrivers: TArray<FNamedNetDriver>                                  
    //          ───────────▲─────────────────────────────                                  
    //                     │                                                               
    //                     │                                                               
    //                     │                                                               
    //            *** World could have multiple NetDrivers:                                
    //                - e.g. IpNetDriver, DemoNetDriver                                    
    //                             ┌───────────┐                                           
    //                             │ NetDriver │                                           
    //                             └────▲──────┘                                           
    //                                  │                                                  
    //                         ┌────────┼──────────┐                                       
    //                         │                   │                                       
    //                  ┌──────┼──────┐    ┌───────┼───────┐                               
    //                  │ IpNetDriver │    │ DemoNetDriver │                               
    //                  └──────▲──────┘    └───────▲───────┘                               
    //                         │                   │                                       
    //                         │                   │                                       
    //                         │              *** DemoNetDriver is used for replay         
    //                         │                                                           
    //                   *** IpNetDriver is "GameNetDriver" which we are interested in     
    //                                                                         
}

/** world this net driver is associated with */
TObjectPtr<class UWorld> World;
TObjectPtr<class UPackage> WorldPackage;

/** interface for communication network state to others (i.e. World usually, buy anything that implements FNetworkNotify) */
class FNetworkNotify* Notify;

/** handles to various registered delegates */
FDelegateHandle TickDispatchDelegateHandle;
FDelegateHandle PostTickDispatchDelegateHandle;
FDelegateHandle TickFlushDelegateHandle;
FDelegateHandle PostTickFlushDelegateHandle;

/** amount of time to wait for a new net connection to be established before destroying the connection */
float InitialConnectTimeout;

/**
    * amount of time to wait before considering an established connection time out
    * typically shorter than the time to wait on a new connection because this connection should already have been setup and any interruption should be trapped quicker
    */
float ConnectionTimeout;

/** connection to the server (this net driver is a client) */
// by ServerConnection's validity, we can know whether this NetDriver is for server or for client
// - for server: ServerConnection == nullptr
// - for client: ServerConnection != nullptr
TObjectPtr<class UNetConnection> ServerConnection;

/** assigns driver unique IDs to client connections */
FNetConnectionIdHandler ConnectionIdHandler;

TSharedPtr<FNetGUIDCache> GuidCache;
};
```

* 띠용? 그런데 궁금한게 Unreal은 UDP이고 Listen이 없지 않나?
    * 그렇다! 이제 Unreal에서 UDP를 이용해 Listen을 어떻게 만드는지 보게 될 것이다!

```cpp                      
virtual bool InitListen(FNetworkNotify* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error) override
{
    // - NetDriver is world's network driver
    // - in the low-level perspective, how to communicate in network?
    // - NetDriver needs socket!!
    //   - in InitBase(), the socket is created for the NetDriver :)
    if (!InitBase(false, InNotify, LocalURL, bReuseAddressAndPort, Error))
    {
        return false;
    }

    InitConnectionlessHandler();

    // update port!
    LocalURL.Port = LocalAddr->GetPort();

    return true;
}
```

```cpp
virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error)
{
    // read any timeout overrides from the URL
    // InitialConnectTimeout and ConnectionTimeout are useful properties to see a glimpse of the reliable UDP
    // - see InitialConnectTimeout and ConnectionTimeout variables briefly
    // - try to understand some portions of reliable UDP with these two variables:
    //
    //   *** Reliable UDP:                                                                            
    //                                                                                                
    //       1. the UDP doesn't have connection: RELIABLE UDP have "connection"                       
    //           - the reliable UDP make a connection in the software manner (not hardware)           
    //             ┌────────┐                               ┌────────┐                                
    //             │        ├───────────────────────────────┤        │                                
    //             │ Client ◄───────────Connection──────────► Server │                                
    //             │        ├───────────────────────────────┤        │                                
    //             └────────┘                               └────────┘                                
    //                                                                                                
    //                 *** when ConnectionTimeout is 1.0, it means for 1 sec:                         
    //                     when there is no packets to communicate for 1 sec, destroy the connection  
    //                                                                        ──────────────────────  
    //                                                                                                
    //       2. the reliable UDP is based on the UDP: the UDP could happen "packet-loss"              
    //           - if it allow endless time to make a connection, it could result in "Conjestion"     
    //                                                                    ───────────────────────     
    //           - how the reliable UDP constructs a connection?: "Handshake" manner similar to TCP   
    //             e.g. 3-way hand-shake in TCP:                   ────────────────────────────────   
    //                                                                                                
    //             ┌────────┐                                         ┌────────┐                      
    //             │ Client │                                         │ Server │                      
    //             └────┬───┘                                         └────┬───┘                      
    //                  │                                                  │                          
    //                  │                                       Prepare ┌──┤                          
    //                  │                                        LISTEN │  │                          
    //                  │                                               └──►                          
    //                  │                                                  │                          
    //                  │             Send SYN (connect request)           │                          
    //        SYN_SENT  ├─────────────────────────────────────────────────►│ LISTEN                   
    //                  │                                                  │                          
    //                  │             Send SYN+ACK                         │                          
    //     ESTABLISHED  │◄─────────────────────────────────────────────────┤ SYN_RECV'ED              
    //     (connection) │                                                  │                          
    //                  │             Send ACK                             │                          
    //                  ├─────────────────────────────────────────────────►│ ESTABLISHED              
    //                  │                                                  │ (connection)             
    //                  │                                                  │                          
    //                  │       *** TCP's 3-way HandShaking                │                          
    //                  │                                                  │                          
    //                  │                                                  │                          
    //                  │                                                  │                          
    //                                                                                                

    if (const TCHAR* InitialConnectTimeoutOverride = URL.GetOption(TEXT("InitialConnectTimeout="), nullptr))
    {
        float ParsedValue;
        LexFromString(ParsedValue, InitialConnectTimeoutOverride);
        if (ParsedValue != 0.f)
        {
            InitialConnectTimeout = ParsedValue;
        }
    }
    if (const TCHAR* ConnectionTimeoutOverride = URL.GetOption(TEXT("ConnectionTimeout="), nullptr))
    {
        float ParsedValue;
        LexFromString(ParsedValue, ConnectionTimeoutOverride);
        if (ParsedValue != 0.0f)
        {
            ConnectionTimeout = ParsedValue;
        }
    }

    // the timeout is very annoying when you debugging, "-NoTimeouts" is very useful cmdarg to remember!
    // - there are multiple ways to prevent the time-outs while you are debugging:
    //   1. use '-NoTimeouts" option
    //   2. set break points on both client-app and server-app
    //
    // - in editor PIE build, when you use the option to run server in same process, you don't have to worry about this
    //   - when you are in debugging mode, the server also in debugging mode!
    if (URL.HasOption(TEXT("NoTimeouts")))
    {
        // 요게 필요한 이유는 Debug시에 브레이크 포인트 잡을경우를 대비해서 필요하다
        bNoTimeouts = true;
    }

    // see InitConnectionClass() ***
    bool bSuccess = InitConnectionClass();
    if (!bInitAsClient)
    {
        // "!bInitAsClient" is 'true' when we are in server-code
        // - we'll cover this ConnectionlessHandler soon!
        ConnectionlessHandler.Reset();
    }

    // note that Notify means World:
    // - UWorld inherits FNetworkNotify!
    Notify = InNotify;

    // see InitNetTraceId() briefly ***
    InitNetTraceId();

    return bSuccess;
}
```

```cpp
virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error) override
{
    // note that IpNetDriver inherits NetDriver, so it is natural to call NetDriver::InitBase()
    //
    // ClientConnect:
    // - no need to see NetDriver::InitBase()
    // - see briefly~ :)
    //   *** focus on InitConnectionClass() call
    if (!UNetDriver::InitBase(bInitAsClient, InNotify, URL, bReuseAddressAndPort, Error))
    {
        return false;
    }

    // in Windows, it returns FSocketSubsystemWindows
    // we already saw SockeySubsystem in the last week (in Week 1)
    ISocketSubsystem* SocketSubsystem = GetSocketSubsystem();
    if (SocketSubsystem == nullptr)
    {
        return false;
    }

    // BindPort is assigned URL.Port (ServerPort) for server, GetClientPort() is called for client side
    const int32 BindPort = bInitAsClient ? GetClientPort() : URL.Port;

    // increase socket queue size, because we are polling rather than threading and thus we rely on the OS socket to buffer a lot of data
    // by default, Desired[Recv|Send]Size are determined like this:
    // - ClientDesiredSocketReceiveBufferBytes(ClientDesiredSocketSendBufferBytes): 32768(32KB)
    // - ServerDesiredSocketReceiveBufferBytes(ServerDesiredSocketSendBufferBytes): 131072(128KB)
    // - Send/Recv is same!

    const int32 DesiredRecvSize = bInitAsClient ? ClientDesiredSocketReceiveBufferBytes : ServerDesiredSocketReceiveBufferBytes;
    const int32 DesiredSendSize = bInitAsClient ? ClientDesiredSocketSendBufferBytes : ServerDesiredSocketSendBufferBytes;

    // first see CreateAndBindSocketsFunc lambda definition before getting into Resolver->InitBindSockets:
    // - actually, this lambda is called inside InitBindSockets()
    // - see what variables are captured in the lambda:
    //   1. BindPort
    //   2. bResueAddressAndPort == false
    //   3. DesiredRecvSize/DesiredSendSize

    const EInitBindSocketsFlags InitBindFlags = bInitAsClient ? EInitBindSocketsFlags::Client : EInitBindSocketsFlags::Server;
    FCreateAndBindSocketFunc CreateAndBindSocketsFunc = [this, BindPort, bReuseAddressAndPort, DesiredRecvSize, DesiredSendSize]
        (TSharedRef<FInternetAddr> BindAddr, FString& Error) -> FUniqueSocket
        {
            return this->CreateAndBindSocket(BindAddr, BindPort, bReuseAddressAndPort, DesiredRecvSize, DesiredSendSize, Error);
        };
    
    // see what the Resolver is ***
    bool bInitBindSocketsSuccess = Resolver->InitBindSockets(MoveTemp(CreateAndBindSocketsFunc), InitBindFlags, SocketSubsystem, Error);
    if (!bInitBindSocketsSuccess)
    {
        return false;
    }

    SetSocketAndLocalAddress(Resolver->GetFirstSocket());

    return true;
}
```

```cpp
/**
    * initialize a PacketHandler for serverside net drivers, for handling connectionless packets
    * NOTE: only triggered by net-driver subclasses that support it - from within InitListen
    */
// as comment said, ConnectionlessHandler is for SERVER, not client
// - this function is only called within the InitListen()
virtual void InitConnectionlessHandler()
{
    // on PacketHandler's constructor, unreal actually pass the pointer of DDos
        // DDos관련 처리를 어떻게 할지는 뺀 상태임 (nullptr) -> 규모가 너무커서 다음에...
    ConnectionlessHandler = MakeUnique<PacketHandler>(nullptr);
    if (ConnectionlessHandler.IsValid())
    {
        // before going further, try to understand what the 'PacketHandler' is:
        //                                                                                                          
        //  *** What is PacketHandler?                                                                              
        //                                                                                                          
        //     ┌───────────────────────┐   │   ┌──────────────────────────────────┐   │                             
        //     │ NetCode(Game-related) │   │   │ Network Engine (Network-related) │   │                             
        //     └───────────────────────┘   │   └──────────────────────────────────┘   │                             
        //                                 │                                          │                             
        //        ┌───────────┐            │      PacketHandler::Incoming()           │                             
        //        │ ┌───────────┐      ◄───┼──────────────────────────────────────────┼──     Actual Network World: 
        //        └─│ ┌─────────┴──┐       │                                          │        e.g. Router          
        //          └─┤  Channels  │   ────┼──────────────────────────────────────────┼──►                          
        //            └────────────┘       │      PacketHandler::Outgoing()           │                             
        //                                 │                                          │                             
        //                                 │                                          │                             
        //                                                                                                          
        //                     *** PacketHandler governs incoming/outgoing packets in unreal engine                 
        // PacketHandler는 NetCode로 들어가는 관문이라 생각하면된다.
        
        // - MAX_PACKET_SIZE is what we covered last time: packet size considering MTU(MSS)
        ConnectionlessHandler->Initialize(UE::Handler::Mode::Server, MAX_PACKET_SIZE, true, nullptr, nullptr, NetDriverDefinition);

        // add handling for the stateless connect handshake, for connectionless packets, as the outermost layer
        // HandlerComponent is a component of PacketHandler
        // - you can think of it as like entity-component
        //   - entity: PacketHandler and component: HandlerComponent
        // - try to understand HandlerComponent conceptually:
        // 
        //   *** The relationship between PacketHandler and HandlerComponents                                                                             
        //                                                                                                                                                
        //     ┌───────────────┐                                                                                                                          
        //     │ PacketHandler │                                                                                                                          
        //     └──┬────────────┘                                                                                                                          
        //        │                                                                                                                                       
        //        └───HandlerComponents: TArray<HandlerComponent>                                                                                         
        //             │                                                                                                                                  
        //             ├───StatelessConnectHandlerComponent                                                                                               
        //             │                                                                                                                                  
        //             ├───EncryptHandlerComponent                                                                                                        
        //             │                                                                                                                                  
        //             └───...                                                                                                                            
        //                                                                                                                                                
        //                                                                                                                                                
        //                    │                          │                                                                                                
        //                    │      PacketHandler       │     Actual Network World:                                                                      
        //                    │                          │                                                                                                
        //                    │     Incoming/Outgoing    │                                                                                                
        //                    │     ┌──────┐  ┌──────┐   │                                                                                                
        //                  ◄─┼─────│      │──│      │───┼─────                                                                                           
        //                  ──┼─────│      │──│      │───┼────►                                                                                           
        //                    │     └───▲──┘  └───▲──┘   │                                                                                                
        //                    │         │         │      │                                                                                                
        //                    │         │         │      │                    ──┐                                                                         
        //                              │         │                             │                                                                         
        //                              │      StatelessConnectHandlerComponent │                                                                         
        //                              │                                       │ you can think of HandlerComponent as gates:                             
        //                              │                                       │  incoming/outgoing packets should take over, reaching to network world! 
        //                            EncrpytHandlerComponent                   │                                                                         
        //                                                                    ──┘                                                                         
        // 
        
        TSharedPtr<HandlerComponent> NewComponent = 
            ConnectionlessHandler->AddHandler(TEXT("Engine.EngineHandlerComponentFactory(StatelessConnectHandlerComponent)"), true);
    
        StatelessConnectComponent = StaticCastSharedPtr<StatelessConnectHandlerComponent>(NewComponent);
        if (StatelessConnectComponent.IsValid())
        {
            StatelessConnectComponent.Pin()->SetDriver(this);
        }

        // we defer initialization of HandlerComponents, here we start to initialize HandlerComponents and make sure PacketHandler to be fully-initialized
        ConnectionlessHandler->InitializeComponents();
    }
}
```