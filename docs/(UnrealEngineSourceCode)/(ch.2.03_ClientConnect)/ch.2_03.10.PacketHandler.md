---
layout: default
title: "03-10. PacketHandler"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## PacketHandler 개요

* PacketHandler는 언리얼 엔진의 네트워크 패킷 처리를 담당하는 중요한 컴포넌트입니다.

### 주요 기능

1. 패킷 처리 파이프라인
    * 네트워크 패킷의 전송 전/후 처리를 담당
    * 암호화, 압축 등 다양한 패킷 가공 작업 수행
    * 핸들러 체인을 통한 모듈식 구조
2. 보안
    * 패킷 암호화/복호화
    * 무결성 검사
    * DDoS 방어 기능
3. 최적화
    * 패킷 압축
    * 대역폭 관리
    * 패킷 우선순위 처리

```cpp
class PacketHandler
{
    // 핸들러 체인 관리
    TArray<TSharedPtr<HandlerComponent>> HandlerComponents;
    
    // 패킷 처리 메서드
    bool ProcessPacket(uint8* Data, int32 Count);
    
    // 핸들러 컴포넌트 추가/제거
    void AddHandler(TSharedPtr<HandlerComponent> NewHandler);
}
```

---

```cpp
/** initializes the PacketHandler */
/** PacketHandler를 초기화합니다 */
virtual void InitHandler()
{
    Handler = MakeUnique<PacketHandler>();
    if (Handler.IsValid())
    {
        // server can create and have NetConnection:
        // - Driver->ServerConnection != nullptr: CLIENT-SIDE: Mode is 'Client'
        // - Driver->ServerConnection == nullptr: SERVER-SIDE: Mode is 'Server'

        // 서버는 NetConnection을 생성하고 가질 수 있습니다:
        // - Driver->ServerConnection != nullptr: 클라이언트 측: Mode는 'Client'
        // - Driver->ServerConnection == nullptr: 서버 측: Mode는 'Server'
        UE::Handler::Mode Mode = Driver->ServerConnection != nullptr ? UE::Handler::Mode::Client : UE::Handler::Mode::Server;
        
        // bind LowLevelSend for PacketHandler with UNetConnection::LowLevelSend
        // - when PacketHandler calls LowLevelSend, its call redirect to UNetConnection::LowLevelSend(or UIpConnection::LowLevelSend)

        // PacketHandler를 위해 LowLevelSend를 UNetConnection::LowLevelSend와 바인딩합니다
        // - PacketHandler가 LowLevelSend를 호출할 때, 이 호출은 UNetConnection::LowLevelSend(또는 UIpConnection::LowLevelSend)로 리다이렉트됩니다
        Handler->InitializeDelegates(FPacketHandlerLowLevelSendTraits::CreateUObject(this, &UNetConnection::LowLevelSend));

        // see PacketHandler::Initialize briefly
        // - we already covered PacketHandler::Initialize()
        // - MaxPacket is in 'bytes' not bits

        // PacketHandler::Initialize를 간단히 살펴보세요
        // - 우리는 이미 PacketHandler::Initialize()를 다뤘습니다
        // - MaxPacket은 비트가 아닌 바이트 단위입니다
        Handler->Initialize(Mode, MaxPacket * 8, false, nullptr, nullptr, Driver->GetNetDriverDefinition());

        // add handling for the stateless connect handshake, for connectionless packets, as the outermost layer
        // same as server-side add handler for StatelessConnectHandlerComponent

        // 연결이 없는 패킷을 위한 무상태 연결 핸드셰이크를 위해 가장 바깥쪽 레이어로 핸들링을 추가합니다
        // 서버 측과 동일하게 StatelessConnectHandlerComponent에 대한 핸들러를 추가합니다
        TSharedPtr<HandlerComponent> NewComponent = Handler->AddHandler(TEXT("Engine.EngineHandlerComponentFactory(StatelessConnectHandlerComponent)"), true);
        
        StatelessConnectComponent = StaticCastSharedPtr<StatelessConnectHandlerComponent>(NewComponent);
        if (StatelessConnectComponent.IsValid())
        {
            StatelessConnectHandlerComponent* CurComponent = StatelessConnectComponent.Pin().Get();

            // we already seen StatelessConnectHandlerComponent::SetDriver, but it has different handling between Mode::Client and Mode::Server
            // - see StatelessConnectComponent::SetDriver (goto 019: ClientConnect)

            // 우리는 이미 StatelessConnectHandlerComponent::SetDriver를 봤지만, Mode::Client와 Mode::Server 간에 다른 처리가 있습니다
            // - StatelessConnectComponent::SetDriver를 살펴보세요 (019: ClientConnect로 이동)
            CurComponent->SetDriver(Driver);
        }

        // different from the server-side, even though we call PacketHandler::InitializeComponents, StatelessConnectHandlerComponent is not initialized:
        // - see StatelessConnectHandlerComponent::Initialize() briefly
        // - note that Mode::Client doesn't call Initialized()!

        // 서버 측과 다르게, PacketHandler::InitializeComponents를 호출해도 StatelessConnectHandlerComponent는 초기화되지 않습니다:
        // - StatelessConnectHandlerComponent::Initialize()를 간단히 살펴보세요
        // - Mode::Client는 Initialize()를 호출하지 않는다는 점에 주의하세요!
        Handler->InitializeComponents();

        // cache maximum packet handler bits to preserve the size of MAX_PACKET_SIZE (MTU) correctly

        // MAX_PACKET_SIZE(MTU)의 크기를 올바르게 유지하기 위해 최대 패킷 핸들러 비트를 캐시합니다
        MaxPacketHandlerBits = Handler->GetTotalReservedPacketBits();
    }
}
```