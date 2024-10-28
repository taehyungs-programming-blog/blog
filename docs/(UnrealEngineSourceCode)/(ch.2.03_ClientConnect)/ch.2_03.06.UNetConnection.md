---
layout: default
title: "03-06. UNetConnection"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UNetConnection 클래스 개요

* UNetConnection은 언리얼 엔진의 네트워크 연결을 담당하는 핵심 클래스입니다. UPlayer 클래스를 상속받아 구현되어 있습니다.

### 주요 특징

1. 연결 관리
    * 클라이언트와 서버 간의 네트워크 연결을 관리
    * 소켓 통신을 처리하고 연결 상태를 유지
2. 패킷 처리
    * 네트워크 패킷의 송수신을 담당
    * 패킷의 신뢰성 있는 전송을 보장
    * 패킷 순서 관리 및 재전송 처리
3. 채널 시스템
    * 여러 개의 채널을 통해 다양한 타입의 데이터 전송
    * 각 채널은 특정 목적(액터 리플리케이션, 음성 채팅 등)을 위해 사용
4. 상태 관리
    * 연결 상태 추적
    * 핑/지연 시간 측정
    * 대역폭 관리
5. 주요 사용 사례   
    * 멀티플레이어 게임에서 클라이언트-서버 통신
    * 게임 상태 동기화
    * 리플리케이션 시스템의 기반
    * RPC(Remote Procedure Calls) 처리
6. 관련 클래스들
    * UNetDriver: 네트워크 드라이버
    * UChannel: 네트워크 채널
    * FOutPacket: 송신 패킷
    * FInPacket: 수신 패킷

이 클래스는 언리얼 엔진의 네트워크 시스템에서 매우 중요한 역할을 하며, 멀티플레이어 게임 개발에 필수적인 요소입니다.

```cpp
class UNetConnection : public UPlayer
```

---

## NetDriver와 UNetConnection 관계

### 기본 관계

1. 소유 관계
    * NetDriver는 여러 UNetConnection 인스턴스들을 소유/관리합니다
    * 각 UNetConnection은 하나의 NetDriver를 참조합니다 (Driver 멤버 변수)
2. 역할
    * NetDriver: 네트워크 시스템의 핵심 관리자
        * 연결 관리
        * 채널 정의 관리 (ChannelDefinitionMap)
        * ConnectionId 할당
        * 전체적인 네트워크 설정 관리 (예: MaxNetTickRate)
    * UNetConnection: 개별 클라이언트와의 연결을 나타냄
        * 채널 관리 (Channels, OpenChannels, ChannelsToTick)
        * 패킷 처리
        * 연결 상태 관리

### 주요 상호작용

1. 초기화 과정

```cpp
void UNetConnection::InitBase(UNetDriver* InDriver, ...)
{
    Driver = InDriver;  // NetDriver 참조 설정
    SetConnectionId(InDriver->AllocateConnectionId());  // NetDriver로부터 ID 할당
    // ...
}
```

2. 채널 생성

```cpp
UChannel* UNetConnection::CreateChannelByName(...)
{
    // NetDriver를 통해 채널 생성
    UChannel* Channel = Driver->GetOrCreateChannelByName(ChName);
    // ...
}
```

3. 타이밍 제어

```cpp
void UNetConnection::Tick(float DeltaSeconds)
{
    // NetDriver의 설정을 사용
    const int32 MaxNetTickRate = Driver->MaxNetTickRate;
    // ...
}
```

### 중요 포인트

1. 계층 구조
    * NetDriver는 전체 네트워크 시스템을 관리
    * UNetConnection은 개별 연결을 관리
    * 각 UNetConnection은 여러 UChannel을 관리
2. 메모리 관리
    * NetDriver가 UNetConnection의 수명주기를 관리
    * UNetConnection이 자신의 채널들의 수명주기를 관리
3. 통신 흐름
    * NetDriver ↔ UNetConnection ↔ UChannel의 계층적 구조로 통신이 이루어짐
    * 각 계층은 자신의 역할에 맞는 기능을 수행

---

```cpp
/** initialize common settings for this connection instance */
/** 이 연결 인스턴스에 대한 공통 설정을 초기화합니다 */
virtual void InitBase(UNetDriver* InDriver, class FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket = 0, int32 InPacketOverhead = 0)
{
    // owning NetDriver
    // NetDriver 소유권 설정
    Driver = InDriver;

    // it looks like UNetConnection holds UChannel related data:
    // UNetConnection이 UChannel 관련 데이터를 보유하고 있는 것으로 보입니다:
    InitChannelData();

    // NetDriver allocate connection id:
    // NetDriver가 연결 ID를 할당합니다:
    SetConnectionId(InDriver->AllocateConnectionId());

    // our state is SOCK_PENDING, when we successfully handshake process, it will be SOCK_OPEN (connection is open)
    // 현재 상태는 SOCK_PENDING이며, 핸드셰이크 프로세스가 성공적으로 완료되면 SOCK_OPEN(연결 열림)이 됩니다
    State = InState;
    MaxPacket = InMaxPacket;

    // create package map
    // we are not going to see the detail of UPackageMapClient for now
    // - but, in terms of replication, PackageMapClient is very important object
    // *** just remember each NetConnection has its own PacketMapClient

    // 패키지 맵 생성
    // 지금은 UPackageMapClient의 세부사항을 살펴보지 않을 것입니다
    // - 하지만 리플리케이션 측면에서 PackageMapClient는 매우 중요한 객체입니다
    // *** 각 NetConnection이 자체 PacketMapClient를 가지고 있다는 점만 기억하세요
    UPackageMapClient* PackageMapClient = NewObject<UPackageMapClient>(this, PackageMapClass);
    {
        PackageMapClient->Initialize(this, Driver->GuidCache);
        PackageMap = PackageMapClient;
    }

    // initialize handler
    // now we finally reach to Handler!
    // - see member variable Handler (PacketHandler)
    // - in server-side, we make PacketHandler and HandlerComponent(StatelessConnectHandlerComponent)
    //   - in cliend-side, NetConnection has PacketHandler matching HandlerComponent(StatelessConnectHandlerComponent)

    // 핸들러 초기화
    // 이제 드디어 Handler에 도달했습니다!
    // - 멤버 변수 Handler(PacketHandler) 참조
    // - 서버 측에서는 PacketHandler와 HandlerComponent(StatelessConnectHandlerComponent)를 생성합니다
    //   - 클라이언트 측에서는 NetConnection이 HandlerComponent(StatelessConnectHandlerComponent)와 매칭되는 PacketHandler를 가집니다
    {
        Handler.Reset();

        InitHandler();
    }
}
```

---

## NetDriver, NetConnection, UChannel의 관계

```
graph TD
    A[NetDriver] --> B[NetConnection]
    B --> C[UChannel]
    
    subgraph "NetDriver (네트워크 관리자)"
        A
        A1[ServerConnection]
        A2[ClientConnections]
    end
    
    subgraph "NetConnection (연결 관리)"
        B
        B1[Channels 배열]
        B2[OpenChannels 배열]
        B3[ChannelsToTick 배열]
    end
    
    subgraph "UChannel (통신 채널)"
        C
        C1[Control Channel]
        C2[Voice Channel]
        C3[Actor Channel]
    end
```

### NetDriver (네트워크 드라이버)

* 네트워크 시스템의 최상위 관리자
* 서버일 경우:
    * ServerConnection: null
* 클라이언트일 경우:
    * ServerConnection: 서버와의 연결을 나타내는 하나의 NetConnection
    * ClientConnections: 비어있음

### NetConnection (네트워크 연결)

* 하나의 클라이언트-서버 간 연결을 나타냄
* 여러 개의 Channel들을 관리:
    * Channels[]: 모든 채널을 인덱스로 관리
    * OpenChannels[]: 현재 열려있는 채널들의 목록
    * ChannelsToTick[]: 매 프레임 업데이트가 필요한 채널들의 목록

### UChannel (채널)

* 특정 타입의 데이터 통신을 담당
* 주요 채널 타입:
    * Control Channel: 연결 제어용 (항상 인덱스 0)
    * Voice Channel: 음성 통신용
    * Actor Channel: 액터 복제용

### 주요 특징

1. 계층 구조:
    * NetDriver → NetConnection → Channel
    * 각 계층이 명확한 역할 분담
2. 채널 시스템:
    * 데이터 종류별로 분리된 채널
    * 효율적인 네트워크 리소스 관리
3. 선택적인 신뢰성 제공
4. 메모리 관리:
    * DefaultMaxChannelSize: 기본 32767
    * 채널은 동적으로 생성/제거됨
5. 틱 최적화:
    * ChannelsToTick을 통해 필요한 채널만 업데이트
6. 불필요한 가상 함수 호출 방지

---

## 주요 함수

```cpp

```