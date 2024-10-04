---
layout: default
title: "03-02. InitHandler"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
/** initializes the PacketHandler */
// create new PacketHandler for UNetConnection
virtual void InitHandler()
{
    Handler = MakeUnique<PacketHandler>();
    if (Handler.IsValid())
    {
        // server can create and have NetConnection:
        // - Driver->ServerConnection != nullptr: CLIENT-SIDE: Mode is 'Client'
        // - Driver->ServerConnection == nullptr: SERVER-SIDE: Mode is 'Server'
        UE::Handler::Mode Mode = Driver->ServerConnection != nullptr ? UE::Handler::Mode::Client : UE::Handler::Mode::Server;
        
        // bind LowLevelSend for PacketHandler with UNetConnection::LowLevelSend
        // - when PacketHandler calls LowLevelSend, its call redirect to UNetConnection::LowLevelSend(or UIpConnection::LowLevelSend)
        // - see birefly PacketHandler::InitializeDelegates and UNetConnection::LowLevelSend
        Handler->InitializeDelegates(FPacketHandlerLowLevelSendTraits::CreateUObject(this, &UNetConnection::LowLevelSend));

        // *** DON'T MISS THIS
        // see PacketHandler::Initialize briefly
        // - we already covered PacketHandler::Initialize()
        // - MaxPacket is in 'bytes' not bits
        Handler->Initialize(Mode, MaxPacket * 8, false, nullptr, nullptr, Driver->GetNetDriverDefinition());

        // add handling for the stateless connect handshake, for connectionless packets, as the outermost layer
        // same as server-side add handler for StatelessConnectHandlerComponent
        TSharedPtr<HandlerComponent> NewComponent = Handler->AddHandler(TEXT("Engine.EngineHandlerComponentFactory(StatelessConnectHandlerComponent)"), true);
        
        StatelessConnectComponent = StaticCastSharedPtr<StatelessConnectHandlerComponent>(NewComponent);
        if (StatelessConnectComponent.IsValid())
        {
            StatelessConnectHandlerComponent* CurComponent = StatelessConnectComponent.Pin().Get();

            // we already seen StatelessConnectHandlerComponent::SetDriver, but it has different handling between Mode::Client and Mode::Server
            CurComponent->SetDriver(Driver);
        }

        // different from the server-side, even though we call PacketHandler::InitializeComponents, StatelessConnectHandlerComponent is not initialized:
        // - see StatelessConnectHandlerComponent::Initialize() briefly
        // - note that Mode::Client doesn't call Initialized()!
        Handler->InitializeComponents();

        // cache maximum packet handler bits to preserve the size of MAX_PACKET_SIZE (MTU) correctly
        MaxPacketHandlerBits = Handler->GetTotalReservedPacketBits();
    }
}
```