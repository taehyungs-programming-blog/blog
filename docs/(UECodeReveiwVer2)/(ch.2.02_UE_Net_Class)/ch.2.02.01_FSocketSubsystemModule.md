---
layout: default
title: "02-01. FSocketSubsystemModule"
parent: "([Network] 02. UE Net의 주요 Class)"
grand_parent: "(UnrealEngine Code-Review Ver.2)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* FSocketSubsystemModule은 언리얼 엔진의 네트워킹 시스템에서 소켓 서브시스템을 관리하는 핵심 모듈

* 주요 특징:
  * 플랫폼별 소켓 시스템 초기화 담당
  * 여러 소켓 서브시스템을 등록하고 관리
  * IModuleInterface를 상속받아 모듈로 동작

```csharp
public class YourProject : ModuleRules
{
    public YourProject(ReadOnlyTargetRules Target) : base(Target)
    {
        // ... 기존 코드 ...
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Sockets",
            "Networking"
        });
    }
}
```

```cpp
class FSocketSubsystemModule : public IModuleInterface
{
    // 기본 소켓 서브시스템 이름
    FName DefaultSocketSubsystem;

    // 등록된 모든 소켓 서브시스템 맵
    TMap<FName, class ISocketSubsystem*> SocketSubsystems;
}
```

```cpp
// 소켓 생성 예시
void CreateNetworkSocket()
{
    // 소켓 서브시스템 가져오기
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    
    // UDP 소켓 생성
    FSocket* Socket = SocketSubsystem->CreateSocket(NAME_DGram, TEXT("UDP Socket"), false);
    
    if (Socket)
    {
        // 바인딩할 주소 설정
        FIPv4Endpoint Endpoint(FIPv4Address::Any, 8080);
        Socket->Bind(*Endpoint.ToInternetAddr());
        
        // 소켓 설정
        Socket->SetNonBlocking(true);
        Socket->SetReuseAddr(true);
    }
}

// 데이터 송신 예시
void SendData(FSocket* Socket, const FString& Data)
{
    if (Socket)
    {
        TArray<uint8> DataArray;
        FTCHARToUTF8 Converted(*Data);
        DataArray.Append((uint8*)Converted.Get(), Converted.Length());
        
        int32 BytesSent = 0;
        Socket->Send(DataArray.GetData(), DataArray.Num(), BytesSent);
    }
}
```

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "NetworkManager.generated.h"

UCLASS()
class YOURPROJECT_API UNetworkManager : public UObject
{
    GENERATED_BODY()

public:
    UNetworkManager();
    virtual ~UNetworkManager();

    // 소켓 초기화
    bool InitializeSocket(const int32 Port);
    
    // 데이터 전송
    bool SendData(const FString& Message, const FString& IP, const int32 Port);
    
    // 데이터 수신
    bool ReceiveData(FString& ReceivedMessage);

private:
    FSocket* Socket;
    bool bIsConnected;
};
```

```cpp
// 게임 인스턴스나 액터에서
UNetworkManager* NetworkManager;

void AYourActor::BeginPlay()
{
    Super::BeginPlay();
    
    NetworkManager = NewObject<UNetworkManager>();
    
    // 소켓 초기화 (포트 8080 사용)
    if (NetworkManager->InitializeSocket(8080))
    {
        // 데이터 전송
        NetworkManager->SendData(TEXT("Hello Server"), TEXT("127.0.0.1"), 8080);
        
        // 데이터 수신
        FString ReceivedMessage;
        if (NetworkManager->ReceiveData(ReceivedMessage))
        {
            UE_LOG(LogTemp, Log, TEXT("Received: %s"), *ReceivedMessage);
        }
    }
}
```