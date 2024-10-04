---
layout: default
title: "([Network] 03. ClientConnect)"
parent: "(UE SourceCode 분석 🤖)"
has_children: true
nav_order: 3
---

```cpp
/** initialize this connection instance ***to*** a remote source */
virtual void InitLocalConnection(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0) override
{
    InitBase(InDriver, InSocket, InURL, InState,
        // use the default packet size/overhead unless overriden by a child class
        // *** note that even though we put InMaxPacket larger than MAX_PACKET_SIZE, it just clamp it!
        // - do NOT modify MAX_PACKET_SIZE arbitrarily!
        (InMaxPacket == 0 || InMaxPacket > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : InMaxPacket,
        // *** 'UDP_HEADER_SIZE' ***
        InPacketOverhead == 0 ? UDP_HEADER_SIZE : InPacketOverhead);
    
    // see where Resolver(NetConnectionAddressResolution) is initialized (UIpConnection::UIpConnection)
    // - note that FNetConnectionAddressResolution is managed by FNetDriverAddressResolution:
    // - we can understand the relationship like this:
    //                                                             *** in general, NetDriverAddressResolution has socket to communicate                                                                      
    //                                                ┌──────────────────────────────────────────────────────────┐                                                                                   
    //                                                │                                                          │                                                                                   
    //      ──────────────────────────────────────────┴───────────                                               │                                                                                   
    //   ┌──FNetDriverAddressResolution : UNetDriver(UIpNetDriver)─────────────────────┐                         │                                                                                   
    //   │                                                                             │                         │                                                                                   
    //   │    ┌──────────────────────────────────────────────────────────────────────┐ │                         │                                                                                   
    //   │    │ FNetConnectionAddressResolution : UNetConnection(UIpConnection) -- 0 ◄─┼─────────┐               │                                                                                   
    //   │    └──────────────────────────────────────────────────────────────────────┘ │         │               │                                                                                   
    //   │                                                                             │         ├───────────────┘                                                                                   
    //   │    ┌──────────────────────────────────────────────────────────────────────┐ │         │     NetDriverAddressResolution's BoundSockets are broadcasted to NetConnectionAddressResolutions  
    //   │    │ FNetConnectionAddressResolution : UNetConnection(UIpConnection) -- 1 ◄─┼─────────┘                                                                                                   
    //   │    └──────────────────────────────────────────────────────────────────────┘ │                                                                                                             
    //   │                                                                             │                                                                                                             
    //   └─────────────────────────────────────────────────────────────────────────────┘                                                                                                             
    //  
                                                                                                                                                                                        
    bool bResolverInit = Resolver->InitLocalConnection(InDriver->GetSocketSubsystem(), InSocket, InURL);
    if (!ResolverInit)
    {
        Close();
        return;
    }

    // UNetConnection::RemoteAddr cached TSharedPtr<FInternetAddr>
    // - RemoteAddr point to same memory address with FNetConnectionAddressResolution::RemoteAddr
    // - currently RemoteAddr is NOT resolved yet, but after resolution is finished, UNetConnection::RemoteAddr can access the resolved InternetAddr value
    RemoteAddr = Resolver->GetRemoteAddr();

    // initialize our send bunch
    InitSendBuffer();
}

```
