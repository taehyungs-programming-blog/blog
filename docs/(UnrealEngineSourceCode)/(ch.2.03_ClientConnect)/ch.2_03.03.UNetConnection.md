---
layout: default
title: "03-03. UNetConnection"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

### `UNetConnection`의 주요 역할

* 네트워크 연결 관리
    * 클라이언트와 서버 간의 연결을 관리하는 핵심 클래스
    * 연결 상태 관리 (USOCK_Invalid, USOCK_Closed, USOCK_Pending, USOCK_Open)
    * 패킷 송수신 처리

* 채널 시스템 관리
    * 여러 종류의 네트워크 통신 채널을 관리
    * 신뢰성 있는 데이터 전송을 위한 시스템 제공

```cpp
TArray<TObjectPtr<UChannel>> Channels;
TArray<int32> OutReliable;
TArray<int32> InReliable;
```

* 패킷 핸들링

```cpp
TUniquePtr<PacketHandler> Handler;
TWeakPtr<StatelessConnectHandlerComponent> StatelessConnectComponent;
```

* UPlayer 상속 이유?
    * PlayerController 연결

```cpp
// UPlayer 클래스의 구조
class UPlayer {
    TObjectPtr<APlayerController> PlayerController;
}
```

* UPlayer를 상속받음으로써 각 네트워크 연결이 특정 PlayerController와 연관될 수 있음
    * 이를 통해 서버는 각 클라이언트 연결을 실제 게임 내 플레이어와 매핑할 수 있음
* 플레이어 관리 구조
    * 서버에서 각 클라이언트 연결을 플레이어 단위로 관리할 수 있게 됨
    * 네트워크 연결과 게임플레이 로직을 자연스럽게 연결하는 구조 제공

---

```cpp
/** state of a connection */
/** 연결의 상태 */
enum EConnectionState
{
    USOCK_Invalid = 0, // connection is invalid, possibly uninitialized
                       // 연결이 유효하지 않음, 아마도 초기화되지 않음
    USOCK_Closed  = 1, // connection permanently closed
                       // 연결이 영구적으로 닫힘
    USOCK_Pending = 2, // connection is awaiting connection
                       // 연결 대기 중
    USOCK_Open    = 3, // connection is open
                       // 연결이 열림
};

class UNetConnection : public UPlayer
{
    UNetConnection()
        : Driver(nullptr)
        , bInternalAck(0)
        , DefaultMaxChannelSize(32767)
        , MaxChannelSize(0)
        , InPacketId(-1)
        , OutPacketId(0) // must be initialized as OutAckPacketId + 1, so loss of first packet can be detected
                         // OutAckPacketId + 1로 초기화되어야 함, 첫 번째 패킷 손실을 감지할 수 있도록
        , OutAckPacketId(-1)
    {}

    uint32 GetConnectionId() const { return ConnectionId; }
    void SetConnectionId(uint32 InConnectionId) { ConnectionId = InConnectionId; }

    void InitChannelData()
    {
        // DefaultMaxChannelSize is fixed on constructor (see the UNetConnection's constructor)
        // - you have option to override this maximum channel size
        // DefaultMaxChannelSize는 생성자에서 고정됩니다 (UNetConnection의 생성자 참조)
        // - 이 최대 채널 크기를 재정의할 수 있는 옵션이 있습니다
        int32 ChannelSize = DefaultMaxChannelSize /** 32767 */;
        MaxChannelSize = ChannelSize;

        // see related member variables briefly
        // 관련 멤버 변수를 간단히 살펴보세요
        Channels.AddDefaulted(ChannelSize);
        OutReliable.AddDefaulted(ChannelSize);
        InReliable.AddDefaulted(ChannelSize);

        // bInternalAck is usually 0:
        // - it it becomes 1, it means all incoming/outgoing packets requested from channels, use reliable UDP:
        //   - which is very inefficient!
        // - only when replay-related NetConnection is set, bInternalAck is enabled as 'true'

        // bInternalAck는 보통 0입니다:
        // - 1이 되면, 채널에서 요청된 모든 수신/발신 패킷이 신뢰할 수 있는 UDP를 사용한다는 의미입니다:
        //   - 이는 매우 비효율적입니다!
        // - 리플레이 관련 NetConnection이 설정된 경우에만 bInternalAck가 'true'로 활성화됩니다
        if (!bInternalAck)
        {
            PendingOutRec.AddDefaulted(ChannelSize);
        }

        PacketNotify.Init(InPacketId, OutPacketId);
    }

    /** initializes the PacketHandler */
    /** PacketHandler를 초기화합니다 */
    virtual void InitHandler()
    {
        // @todo
    }

    /** initialize common settings for this connection instance */
    /** 이 연결 인스턴스에 대한 공통 설정을 초기화합니다 */
    // 011 - Networking - ClientConnect * - UNetConnection::InitBase
    virtual void InitBase(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0)
    {
        // owning NetDriver
        // see member variable Driver breifly

        // 소유하는 NetDriver
        // Driver 멤버 변수를 간단히 살펴보세요
        Driver = InDriver;

        // it looks like UNetConnection holds UChannel related data:
        // - see InitChannelData (***)
        // UNetConnection이 UChannel 관련 데이터를 보유하는 것 같습니다:
        // - InitChannelData를 참조하세요 (***)
        InitChannelData();

        // NetDriver allocate connection id:
        // - see UNetDriver::AllocateConnectionId
        // NetDriver가 연결 ID를 할당합니다:
        // - UNetDriver::AllocateConnectionId를 참조하세요
        SetConnectionId(InDriver->AllocateConnectionId());

        // our state is SOCK_PENDING, when we successfully handshake process, it will be SOCK_OPEN (connection is open)
        // 우리의 상태는 SOCK_PENDING이며, 핸드셰이크 과정이 성공적으로 완료되면 SOCK_OPEN(연결 열림)이 됩니다
        State = InState;
        MaxPacket = InMaxPacket;

        // create package map
        // 패키지 맵 생성
        // we are not going to see the detail of UPackageMapClient for now
        // - but, in terms of replication, PackageMapClient is very important object
        // - just remember each NetConnection has its own PacketMapClient
        // - let's see just glimpse of UPackageMapClient

        // 지금은 UPackageMapClient의 세부 사항을 보지 않을 것입니다
        // - 하지만 복제 측면에서 PackageMapClient는 매우 중요한 객체입니다
        // - 각 NetConnection이 자체 PacketMapClient를 가지고 있다는 점만 기억하세요
        // - UPackageMapClient를 간단히 살펴보겠습니다
        UPackageMapClient* PackageMapClient = NewObject<UPackageMapClient>(this, PackageMapClass);
        {
            PackageMapClient->Initialize(this, Driver->GuidCache);
            PackageMap = PackageMapClient;
        }

        // initialize handler
        // 핸들러 초기화
        // now we finally reach to Handler!
        // - see member variable Handler (PacketHandler)
        // - in serverside, we make PacketHandler and HandlerComponent(StatelessConnectHandlerComponent)
        //   - in cliend side, NetConnection has PacketHandler matching HandlerComponent(StatelessConnectHandlerComponent)

        // 이제 드디어 Handler에 도달했습니다!
        // - Handler 멤버 변수(PacketHandler)를 참조하세요
        // - 서버 측에서는 PacketHandler와 HandlerComponent(StatelessConnectHandlerComponent)를 만듭니다
        //   - 클라이언트 측에서는 NetConnection이 HandlerComponent(StatelessConnectHandlerComponent)와 일치하는 PacketHandler를 가집니다

        {
            Handler.Reset();
            InitHandler();
        }
    }

    /** state: this connection is in */
    /** 상태: 이 연결의 현재 상태 */
    EConnectionState State;

    /** maximum packet size */
    /** 최대 패킷 크기 */
    int32 MaxPacket;

    /** owning net driver */
    /** 소유하는 넷 드라이버 */
    TObjectPtr<class UNetDriver> Driver;

    /** the class name for the PackageMap to be loaded */
    /** 로드될 PackageMap의 클래스 이름 */
    TSubclassOf<UPackageMap> PackageMapClass;

    /** package map between local and remote (negotiate net serialization) */
    /** 로컬과 원격 간의 패키지 맵 (네트워크 직렬화 협상) */
    TObjectPtr<class UPackageMap> PackageMap;

    /** packet handler: for managing layered handler components, which modify packets as they are sent/received */
    /** 패킷 핸들러: 전송/수신 시 패킷을 수정하는 계층화된 핸들러 컴포넌트 관리용 */
    TUniquePtr<PacketHandler> Handler;

    /** reference to the PacketHandler component, for managing stateless connection handshakes */
    /** 무상태 연결 핸드셰이크를 관리하기 위한 PacketHandler 컴포넌트에 대한 참조 */
    TWeakPtr<StatelessConnectHandlerComponent> StatelessConnectComponent;

    /** sequence data used to implement reliability */
    /** 신뢰성 구현에 사용되는 시퀀스 데이터 */
    FNetPacketNotify PacketNotify;

    /** internally ack all packets, for 100% reliable connections */
    /** 100% 신뢰할 수 있는 연결을 위해 내부적으로 모든 패킷을 확인 */
    uint32 bInternalAck : 1; 

    /**
     * channel table
     * 채널 테이블
     */
    int32 DefaultMaxChannelSize;
    int32 MaxChannelSize;

    TArray<TObjectPtr<UChannel>> Channels;
    TArray<int32> OutReliable;
    TArray<int32> InReliable;

    /**
     * packet
     * 패킷
     */

    /** full incoming packet index */
    /** 전체 수신 패킷 인덱스 */
    int32 InPacketId; 
    /** most recently sent packet */
    /** 가장 최근에 전송된 패킷 */
    int32 OutPacketId;
    /** most recently acked outgoing packet */
    /** 가장 최근에 확인된 발신 패킷 */
    int32 OutAckPacketId;
};
```

```cpp
// UPlayer has APlayerController!
// - UNetConnection could have PlayerController~ (o o)?
class UPlayer : public UObject, public FExec
{
    /** the actor this player controls */
    // UPlayer has PlayerController
    TObjectPtr<APlayerController> PlayerController;
};
```