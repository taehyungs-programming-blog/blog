---
layout: default
title: "02-04. EnableListenServer"
parent: "([Network] 02. ServerListen)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* Listen의 시작!
* (주의) `EnableListenServer`는 GameInstance의 함수입니다.
* GameInstance과 World의 NetWork 관리 체계
    * GameInstance - 
        * 게임 전체의 수명주기를 관리
        *   여러 World/Level 간의 영속성 있는 데이터와 상태를 유지
        * 네트워크 연결의 고수준 설정을 담당 (예: EnableListenServer)
    * World - 
        * 실제 게임플레이가 일어나는 공간을 관리
        * Actor, Component 등 게임플레이 요소들의 컨테이너
        * 실제 네트워크 통신의 저수준 구현을 담당 (NetDriver, Listen 등)    
    
```
GameInstance (고수준)
    ↓
    - 네트워크 설정 관리
    - 연결 상태 유지
    - World 간 전환 관리
    
World (저수준)
    ↓ 
    - 실제 네트워크 통신 처리
    - NetDriver 관리
    - 리플리케이션 처리
```

* 실전으로 보자!

```cpp
class UGameInstance : public UObject, public FExec
{

// ...

/**
    * turns on/off listen server capability for a game instance
    * - by default, this will set up the persistent URL state so it persists across server travels and spawn the appropriate network listener
    */
/**
    * 게임 인스턴스에 대한 리슨 서버 기능을 켜거나 끕니다
    * - 기본적으로 이는 서버 이동 간에 지속되는 영구 URL 상태를 설정하고 적절한 네트워크 리스너를 생성합니다
    */
virtual bool EnableListenServer(bool bEnable, int32 PortOverride = 0)
{
    UWorld* World = GetWorld();
    ENetMode ExistingMode = World->GetNetMode();

    // our world has its NetMode as 'NM_DedicatedServer', cuz we set bRunAsDedicated 'true'
    // - client can't listen server also!
    // - I recommend you to see where NM_DedicatedServer is set

    // 우리의 월드는 NetMode가 'NM_DedicatedServer'입니다. bRunAsDedicated를 'true'로 설정했기 때문입니다
    // - 클라이언트도 리슨 서버가 될 수 없습니다!
    // - NM_DedicatedServer가 어디서 설정되는지 확인해보는 것을 추천합니다
    if (ExistingMode == NM_Client)
    {
        // clients cannot change to listen
        // 클라이언트는 리슨으로 변경할 수 없습니다
        return false;
    }

    int32 DefaultListenPort = FURL::UrlConfig.DefaultPort;
    if (bEnable)
    {
        // modify the persistent URL
        // in previous callstack, we set 'PortOverride', so it'll be overriden
        // - by adding URL as "Listen", we mark world to listen

        // 영구 URL 수정
        // 이전 콜스택에서 'PortOverride'를 설정했으므로, 이것이 덮어씌워질 것입니다
        // - URL에 "Listen"을 추가함으로써, 월드가 리슨하도록 표시합니다
        if (PortOverride != 0)
        {
            WorldContext->LastURL.Port = PortOverride;
        }
        WorldContext->LastURL.AddOption(TEXT("Listen"));

        // we didn't have server's world net-driver, need to create new one
        // 서버의 월드 넷 드라이버가 없었으므로, 새로 생성해야 합니다
        if (!World->GetNetDriver())
        {
            // this actually opens the port
            // 실제로 포트를 엽니다
            FURL ListenURL = WorldContext->LastURL;

            // Listen의 시작!
            return World->Listen(ListenURL);
        }
        else
        {
            // already listening
            // 이미 리슨 중입니다
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
class UWorld final : public UObject, public FNetworkNotify
{
    // ...
    
/** start listening for connections */
/** 연결 수신 대기 시작 */
bool Listen(FURL& InURL)
{
    // as you can see Listen()'s code only compiled with server-build
    // - by wrapping-up WITH_SERVER_CODE, we can hide server-side codes to client-binary!
    // - highly recommended to wrap WITH_SERVER_CODE for server-side code in your content-code, for security purpose
    // - in MMORPG, it is VERY VERY important to prevent server's binary or pdb from leaking!
    // - in dedicated server of unreal, it is totally same!
    // *** hiding server code is the first step to fight with hackers (cheating companys)
    // *** share typical MMORPG server-side anti-cheat cases

    // 보시다시피 Listen() 코드는 서버 빌드에서만 컴파일됩니다
    // - WITH_SERVER_CODE로 감싸서 서버 측 코드를 클라이언트 바이너리에서 숨길 수 있습니다!
    // - 보안을 위해 콘텐츠 코드에서 서버 측 코드를 WITH_SERVER_CODE로 감싸는 것을 강력히 권장합니다
    // - MMORPG에서는 서버의 바이너리나 pdb가 유출되는 것을 방지하는 것이 매우 매우 중요합니다!
    // - 언리얼의 전용 서버에서도 완전히 동일합니다!
    // *** 서버 코드를 숨기는 것은 해커들(치팅 회사들)과 싸우는 첫 번째 단계입니다
    // *** 전형적인 MMORPG 서버 측 안티치트 사례 공유
#if WITH_SERVER_CODE || 1
    // create net driver
    // - "GameNetDriver" == NAME_GameNetDriver

    // 네트워크 드라이버 생성
        // CreateNamedNetDriver 는 다음장에서 추가 설명.
    // - "GameNetDriver" == NAME_GameNetDriver
    if (GEngine->CreateNamedNetDriver(this, NAME_GameNetDriver, NAME_GameNetDriver))
    {
        // we should find NetDriver(IpNetDriver)
        // NetDriver(IpNetDriver)를 찾아야 합니다
        NetDriver = GEngine->FindNamedNetDriver(this, NAME_GameNetDriver);

        // SetWorld 다다음장 참조
        NetDriver->SetWorld(this);
    }

    if (NetDriver == nullptr)
    {
        // netdriver creation is failed
        // 네트워크 드라이버 생성 실패
        return false;
    }

    FString Error;
    // InitListen 다다다음장 참조
    if (!NetDriver->InitListen(this, InURL, false, Error))
    {
        return false;
    }

    return true;
#endif
}
```