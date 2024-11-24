---
layout: default
title: "02-01. UNetDriver"
parent: "([Network] 01. UE Network 설계)"
grand_parent: "(UnrealEngine Code-Review 2025 Ver. 🐍)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UNetDriver 설계 이념

* UNetDriver는 언리얼 엔진의 네트워크 시스템에서 가장 상위 수준의 관리자 역할을 합니다. 주요 설계 원칙은 다음과 같습니다:

* 단일 책임 원칙 (SRP)
    * **네트워크 연결의 생성과 관리만을 담당** : 요게 핵심이겠지?
    * 서버와 클라이언트 모두에서 동작하는 단일 인터페이스 제공
* 추상화 계층
    * 하위 네트워크 계층의 복잡성을 숨김
    * 플랫폼 독립적인 네트워크 인터페이스 제공
* 생명주기 관리
    * 연결의 시작부터 종료까지 전체 생명주기 관리
    * 타임아웃과 연결 상태 모니터링

* 결론 : NetWork 관리를 UNetDriver에게 모두 일임한다.
    * 단, Tick은 받아야하니 World에서 UNetDriver를 찾아서 Tick을 부르는게 좋겠지?

```cpp
class ENGINE_API UNetDriver : public UObject
{
    GENERATED_BODY()

protected:
    // 서버일 경우 모든 클라이언트 연결 관리
    TArray<UNetConnection*> ClientConnections;
    
    // 클라이언트일 경우 서버와의 연결
    UNetConnection* ServerConnection;

    // 월드 컨텍스트
    class UWorld* World;
    
    // 네트워크 설정
    FNetworkNotify* NotifyHandler;
    double ConnectionTimeout;
    
public:
    // 초기화 및 정리
    virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error);
    virtual void Shutdown();
    
    // 연결 관리
    virtual class UNetConnection* NewConnection(const FNetworkNotify& InNotify);
    virtual void RemoveClientConnection(UNetConnection* ClientConnection);
    
    // 네트워크 업데이트
    virtual void TickDispatch(float DeltaTime);
    virtual void TickFlush(float DeltaTime);
    
    // 채널 시스템
    virtual UChannel* CreateChannelByName(const FName& ChName, EChannelCreateFlags CreateFlags);
    
    // 리플리케이션
    virtual void ReplicateActors();
    
protected:
    // 연결 상태 관리
    virtual void CheckClientConnections();
    virtual bool IsNetResourceValid();
    
    // 패킷 처리
    virtual void ProcessRemoteFunction(class AActor* Actor, class UFunction* Function, void* Parameters, struct FOutParmRec* OutParms, struct FFrame* Stack, class UObject* SubObject = NULL);
    
    // 디버깅
    virtual void LowLevelSend(FOutPacketTraits& Traits, const uint8* Data, int32 CountBits);
    
    UPROPERTY()
    FNetworkNotify* NotifyHandler;
    
    // 네트워크 통계
    FNetworkStats NetworkStats;
};
```

```cpp
class UGameInstance
{
    // 네트워크 게임 시작 시
    virtual bool CreateNetDriver()
    {
        // NetDriver 생성
        UNetDriver* NewDriver = NewObject<UNetDriver>();
        
        if (NewDriver && NewDriver->InitBase(...))
        {
            World->SetNetDriver(NewDriver);
            return true;
        }
        return false;
    }
}

// 실제 호출 예시
void UGameInstance::StartNetGame()
{
    if (CreateNetDriver())
    {
        // 네트워크 게임 시작
        GetWorld()->Listen(); // 서버
        // 또는
        GetWorld()->ClientTravel(...); // 클라이언트
    }
}
```