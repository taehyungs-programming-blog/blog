---
layout: default
title: "05. Actor Replication"
parent: "(Network)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/5)

## Actor Replication

* 서버 혹은 클라이언트의 Actor를 다른 클라이언트 플레이어에 복사하는 작업
    *  네트워크를 통해 여러 플레이어 간에 게임 월드의 상태를 동기화 라 생각하면 편하
    * 대부분 서버에서 클라로 복사
* 크게 두 가지로 나뉨
    * **Property Replication**: 변수의 복사라 생각하면 편함
    * **RPCs(Remote Procedure Calls)**: 함수의 복사라 생각하면 편함
* 대략적 예시코드는 아래와 같음. (자세한 사항은 뒤에서 다룰 예정)

---

## Property Replication

* 게임의 상태를 나타내는 변수(예: 캐릭터의 위치, 체력 등)를 네트워크를 통해 다른 클라이언트와 서버로 복제하는 과정입니다.

```cpp
// Example-1
UPROPERTY(ReplicatedUsing = OnRep_ServerRotationYaw)
float ServerRotationYaw;

UFUNCTION()
void OnRep_ServerRotationYaw();


// .cpp
    // 서버와 클라를 동기화 해준다
/*
* GetLifetimeReplicatedProps 함수는 네트워크에서 객체의 변수들을 어떻게 복제(Replicate)할지 결정하는 데 사용됩니다. 
* 이 함수는 특히 멀티플레이어 게임에서 중요한데, 여러 클라이언트 간에 게임 상태를 일관되게 유지하기 위해 사용됩니다.
*/
void Class::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Class, ServerRotationYaw);
}

// 콜백처리
void Class::OnRep_ServerRotationYaw()
{
	FRotator NewRotator = RootComponent->GetComponentRotation();
	NewRotator.Yaw = ServerRotationYaw;
	RootComponent->SetWorldRotation(NewRotator);
}
```

```cpp
// Example-2

// .h file
#include "GameFramework/Actor.h"
#include "MyReplicatedActor.generated.h"

UCLASS()
class MYGAME_API AMyReplicatedActor : public AActor
{
    GENERATED_BODY()

public:
    // 변수를 Replicated로 설정
    UPROPERTY(Replicated)
    int32 MyReplicatedProperty;
    
    // 복제를 위한 함수 선언
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};

// .cpp file
#include "MyReplicatedActor.h"
#include "Net/UnrealNetwork.h"

void AMyReplicatedActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 이 변수를 복제하도록 설정
    DOREPLIFETIME(AMyReplicatedActor, MyReplicatedProperty);
}

```

---

## Remote Procedure Calls (RPCs)

* RPCs는 네트워크를 통해 다른 컴퓨터에 있는 함수를 호출할 수 있게 해주는 메커니즘입니다. Unreal에서는 세 가지 유형의 RPC가 있습니다: 
    * UFUNCTION 매크로와 함께 Client, Server, Multicast 키워드를 사용하여 정의됩니다.

* **Client RPCs**: 서버에서 클라이언트로 함수를 호출합니다.
* **Server RPCs**: 클라이언트에서 서버로 함수를 호출합니다.
* **Multicast RPCs**: 서버에서 모든 클라이언트로 함수를 호출합니다.

```cpp
// .h file
UCLASS()
class MYGAME_API AMyRPCActor : public AActor
{
    GENERATED_BODY()

public:
    // 클라이언트에서 서버로 호출되는 RPC 함수
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerDoSomething(int32 Value);

    // 서버에서 클라이언트로 호출되는 RPC 함수
    UFUNCTION(Client, Reliable)
    void ClientDoSomething(int32 Value);
};

// .cpp file
void AMyRPCActor::ServerDoSomething_Implementation(int32 Value)
{
    // 서버에서 실행될 코드
}

bool AMyRPCActor::ServerDoSomething_Validate(int32 Value)
{
    // 입력 값을 검증할 수 있는 코드
    return true; // 검증이 성공하면 true를 반환
}

void AMyRPCActor::ClientDoSomething_Implementation(int32 Value)
{
    // 클라이언트에서 실행될 코드
}

```

* `UFUNCTION` 선언규칙
* **Server**:
    * 이 키워드는 함수가 클라이언트에서 서버로 호출되어야 함을 나타냅니다. 
    * 즉, 이 함수는 클라이언트가 실행하며, 그 **실행 결과는 서버에 의해 처리**됩니다.
* **Reliable**:
    * Reliable은 네트워크를 통해 이 함수가 반드시 도달하고 실행되어야 함을 **보장**합니다. 
    * 즉, 호출이 손실되지 않도록 네트워크가 이를 **재전송할 책임**을 집니다.
* **WithValidation**:
    * WithValidation은 서버에 도달하기 전 클라이언트에서 호출된 함수가 유효한지 검증하는 데 사용됩니다. 이는 서버가 불필요한 작업을 수행하는 것을 방지하고, 잘못된 데이터나 악의적인 호출로부터 서버를 보호합니다.
