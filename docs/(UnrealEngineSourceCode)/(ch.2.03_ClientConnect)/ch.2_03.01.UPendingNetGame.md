---
layout: default
title: "03-01. UPendingNetGame"
parent: "([Network] 03. ClientConnect)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

### `UPendingNetGame`의 주요 역할

* 클라이언트 연결 관리
    * 클라이언트가 서버에 연결을 시도할 때 사용되는 임시 객체입니다
    * 서버 연결 과정에서 필요한 NetDriver를 생성하고 관리합니다

* 연결 상태 추적

```cpp
bool bSuccessfullyConnected;  // 서버 연결 성공 여부
bool bSentJoinRequest;        // Join 요청 전송 여부
bool bLoadedMapSuccessfully;  // 맵 로딩 완료 여부
```

* 네트워크 전환 단계 관리
    * UWorld로 전환되기 전의 중간 단계 역할을 합니다
    * 연결 성공 → 맵 로딩 → UWorld 전환 과정을 관리합니다
    
* 네트워크 드라이버 초기화
    * 클라이언트용 IpNetDriver를 생성하고 초기화합니다
    * 이 NetDriver는 연결 성공 후에도 재사용됩니다

---

```cpp
// note that UPendingNetGame inherits from FNetworkNotify which is similar to UWorld
// UPendingNetGame가 UWorld와 유사한 FNetworkNotify를 상속받는다는 점에 주목하세요
class UPendingNetGame : public UObject, FNetworkNotify
{
    // ... 기존 코드 ...

    void InitNetDriver()
    {
        // try to create NetDriver
        // we create client IpNetDriver with UPendingNetGame
        // NAME_PendingNetDriver == "PendingNetDriver"
        // NAME_GameNetDriver == "GameNetDriver"
        // - CreateNamedNetDriver pass UPendingNetGame not UWorld (different from server)

        // UPendingNetGame를 사용하여 클라이언트 IpNetDriver를 생성합니다
        // NAME_PendingNetDriver == "PendingNetDriver"
        // NAME_GameNetDriver == "GameNetDriver"
        // - CreateNamedNetDriver는 UWorld가 아닌 UPendingNetGame를 전달합니다 (서버와 다름)

        // CreateNamedNetDriver (1) 참조
        if (GEngine->CreateNamedNetDriver(this, NAME_PendingNetDriver, NAME_GameNetDriver))
        {
            NetDriver = GEngine->FindNamedNetDriver(this, NAME_PendingNetDriver);
        }

        // see IpNetDriver::InitConnect
        // IpNetDriver::InitConnect를 참조 (2)
        if (NetDriver->InitConnect(this, URL, ConnectionError))
        {
            // ...
        }
    }

    /** URL associated with this level */
    /** 이 레벨과 연관된 URL */
    FURL URL;

    /**
     * NetDriver created for contacting the new server
     * transferred to world on successful connection
     */
    /**
     * 새 서버에 연결하기 위해 생성된 NetDriver
     * 연결 성공 시 월드로 전송됨
     */
    // as comments said, we use NetDriver while connecting to server, after that, we still reuse this NetDriver
    // 주석에서 언급했듯이, 서버에 연결하는 동안 NetDriver를 사용하고, 그 후에도 이 NetDriver를 계속 재사용합니다
    TObjectPtr<class UNetDriver> NetDriver;

    // it could be spoiler, but I'd like to mention that, after successfully connect to the server, we still need another stage 'JOIN' getting into gameplay
    // 스포일러가 될 수 있지만, 서버에 성공적으로 연결한 후에도 게임플레이에 진입하기 위해 'JOIN' 단계가 필요하다는 점을 언급하고 싶습니다
    bool bSuccessfullyConnected;
    bool bSentJoinRequest;

    /** set when we call LoadMapCompleted */
    // when we finally loading map, we'll transition from UPendingNetGame to UWorld(loaded map)

    /** LoadMapCompleted를 호출할 때 설정됨 */
    // 맵 로딩이 완료되면 UPendingNetGame에서 UWorld(로드된 맵)로 전환됩니다
    bool bLoadedMapSuccessfully;

    FString ConnectionError;
};
```

## (1) CreateNamedNetDriver

```cpp
bool CreateNamedNetDriver(UPendingNetGame *PendingNetGame, FName NetDriverName, FName NetDriverDefinition)
{
    return CreateNamedNetDriver_Local(this, GetWorldContextFromPendingNetGameChecked(PendingNetGame), NetDriverName, NetDriverDefinition);
}
```

## (2) InitConnect

```cpp
virtual bool InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error) override
{
    // we pass "bInitAsClient == true": (in server-side code, we pass it as 'false')
    // - see UIpNetDriver::InitBase caring about bInitAsClient == ture,

    // "bInitAsClient == true"로 전달합니다: (서버 측 코드에서는 'false'로 전달합니다)
    // - UIpNetDriver::InitBase에서 bInitAsClient == true에 대해 처리하는 부분을 확인하세요 
    if (!InitBase(true, InNotify, ConnectURL, false, Error))
    {
        return false;
    }

    // create new connection
    // NetConnectionClass is 'IpNetConnection'
    // - see UNetDriver::ServerConnection (***)
    // - see UIpConnection
    //   - note that NetConnectionClass is loaded in InitConnectionClass()

    // 새 연결 생성
    // NetConnectionClass는 'IpNetConnection'입니다
    // - UNetDriver::ServerConnection 참조 (***)
    // - UIpConnection 참조
    //   - NetConnectionClass는 InitConnectionClass()에서 로드된다는 점에 주목하세요
    ServerConnection = NewObject<UNetConnection>(GetTrasientPackage(), NetConnectionClass);

    ServerConnection->InitLocalConnection(this, SocketPrivate.Get(), ConnectURL, USOCK_Pending);
    
}
```