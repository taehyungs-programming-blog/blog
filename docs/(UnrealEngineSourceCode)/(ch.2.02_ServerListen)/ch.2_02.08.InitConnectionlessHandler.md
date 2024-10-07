---
layout: default
title: "02-08. InitConnectionlessHandler"
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
/**
    * initialize a PacketHandler for serverside net drivers, for handling connectionless packets
    * NOTE: only triggered by net-driver subclasses that support it - from within InitListen
    */
/**
    * 서버 측 넷 드라이버를 위한 PacketHandler를 초기화하여 연결 없는 패킷을 처리합니다.
    * 참고: InitListen 내에서 이를 지원하는 넷 드라이버 하위 클래스에 의해서만 트리거됩니다.
    */
// as comment said, ConnectionlessHandler is for SERVER, not client
// - this function is only called within the InitListen()
// 주석에서 언급했듯이, ConnectionlessHandler는 서버용이며 클라이언트용이 아닙니다.
// - 이 함수는 InitListen() 내에서만 호출됩니다.
virtual void InitConnectionlessHandler()
{
    // on PacketHandler's constructor, unreal actually pass the pointer of DDos
    // PacketHandler의 생성자에서 언리얼은 실제로 DDoS의 포인터를 전달합니다.
    ConnectionlessHandler = MakeUnique<PacketHandler>(nullptr);
    if (ConnectionlessHandler.IsValid())
    {
        // before going further, try to understand what the 'PacketHandler' is:
        // 더 진행하기 전에 'PacketHandler'가 무엇인지 이해해 봅시다:
        //                                                                                                          
        //  *** What is PacketHandler?                                                                              
        //  *** PacketHandler란 무엇인가?                                                                              
        //                                                                                                          
        //     ┌───────────────────────┐   │   ┌──────────────────────────────────┐   │                             
        //     │ NetCode(Game-related) │   │   │ Network Engine (Network-related) │   │                             
        //     │ NetCode(게임 관련)     │   │   │ 네트워크 엔진 (네트워크 관련)    │   │                             
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
        //                     *** PacketHandler는 언리얼 엔진에서 들어오고 나가는 패킷을 관리합니다                 
        //                                       ──────────────────────────────────                                 
        //                                                                                                          
        

        // - MAX_PACKET_SIZE: MTU(MSS)를 고려한 패킷 크기
        ConnectionlessHandler->Initialize(UE::Handler::Mode::Server, MAX_PACKET_SIZE, true, nullptr, nullptr, NetDriverDefinition);

        // add handling for the stateless connect handshake, for connectionless packets, as the outermost layer
        // HandlerComponent is a component of PacketHandler
        // - you can think of it as like entity-component
        //   - entity: PacketHandler and component: HandlerComponent
        // - try to understand HandlerComponent conceptually:

        // 연결 없는 패킷을 위해 무상태 연결 핸드셰이크 처리를 가장 바깥 레이어로 추가합니다
        // HandlerComponent는 PacketHandler의 구성 요소입니다
        // - 이를 엔티티-컴포넌트와 같은 개념으로 생각할 수 있습니다
        //   - 엔티티: PacketHandler, 컴포넌트: HandlerComponent
        // - HandlerComponent를 개념적으로 이해해 봅시다:
        // 
        //   *** The relationship between PacketHandler and HandlerComponents                                                                             
        //   *** PacketHandler와 HandlerComponents 간의 관계                                                                             
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
        //                    │                          │     실제 네트워크 세계:                                                                      
        //                    │     Incoming/Outgoing    │                                                                                                
        //                    │     들어오는/나가는      │                                                                                                
        //                    │     ┌──────┐  ┌──────┐   │                                                                                                
        //                  ◄─┼─────│      │──│      │───┼─────                                                                                           
        //                  ──┼─────│      │──│      │───┼────►                                                                                           
        //                    │     └───▲──┘  └───▲──┘   │                                                                                                
        //                    │         │         │      │                                                                                                
        //                    │         │         │      │                    ──┐                                                                         
        //                              │         │                             │                                                                         
        //                              │      StatelessConnectHandlerComponent │                                                                         
        //                              │                                       │ you can think of HandlerComponent as gates:                             
        //                              │                                       │ HandlerComponent를 게이트로 생각할 수 있습니다:                             
        //                              │                                       │  incoming/outgoing packets should take over, reaching to network world! 
        //                              │                                       │  들어오고 나가는 패킷이 네트워크 세계에 도달하기 위해 통과해야 합니다! 
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
        // HandlerComponents의 초기화를 지연시켰습니다. 여기서 HandlerComponents를 초기화하고 PacketHandler가 완전히 초기화되도록 합니다
        ConnectionlessHandler->InitializeComponents();
    }
}
```

* `PacketHandler`에 대한 설명은 다음장에서 조금 더 해보자

