---
layout: default
title: "02-06. NetDriver::SetWorld"
parent: "([Network] 02. ServerListen)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* `NetDriver`클래스의 함수임을 기억하자

```cpp
/**
    * associate a world with this net driver
    * disassociation any previous world first
    */
/**
    * 이 네트워크 드라이버와 월드를 연결합니다
    * 먼저 이전 월드와의 연결을 해제합니다
    */

// UNetDriver::SetWorld() is one of important methods you need to know:
// - this method links tick-events of NetDriver to the world
// UNetDriver::SetWorld()는 알아야 할 중요한 메서드 중 하나입니다:
// - 이 메서드는 NetDriver의 틱 이벤트를 월드에 연결합니다
virtual void SetWorld(UWorld* InWorld)
{
    // first, unlink all previously bound netdriver's tick events
    // 먼저, 이전에 바인딩된 모든 netdriver의 틱 이벤트를 해제합니다
    if (World)
    {
        // remove old world association
        // 이전 월드 연결 제거
        UnregisterTickEvents(World);
        World = nullptr;
        WorldPackage = nullptr;
        Notify = nullptr;
    }

    // let's see what kind of tick-events need to be bound to the world's tick
    // 월드의 틱에 어떤 종류의 틱 이벤트를 바인딩해야 하는지 살펴봅시다
    if (InWorld)
    {
        // setup new world association
        // 새로운 월드 연결 설정
        World = InWorld;
        WorldPackage = InWorld->GetOutermost();
        Notify = InWorld;
        RegisterTickEvents(InWorld);
    }

    // try to understand the relationship between World and NetDriver:
    // World와 NetDriver 사이의 관계를 이해해 봅시다:
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
    //            *** 월드는 여러 NetDriver를 가질 수 있습니다:                                
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
    //                         │              *** DemoNetDriver는 리플레이에 사용됩니다         
    //                         │                                                           
    //                   *** IpNetDriver is "GameNetDriver" which we are interested in     
    //                   *** IpNetDriver는 우리가 관심 있는 "GameNetDriver"입니다     
    //                                                                         
}
```