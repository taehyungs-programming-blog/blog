---
layout: default
title: "03. Connection & OwnerShip"
parent: "(Network)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/3)

## 용어정리

* **Connection**: 모든 데이터를 전달하는 네트워크 통로
* **Packet**: 말 그대로 데이터 패킷
* **Channel**: 데이터를 전달하는 통로
* **Bunch**: 하나의 명령에 대응하는 데이터 묶

* **Connection**: 
    * 이는 네트워크의 두 지점 사이의 연결을 나타냅니다. 
    * 예를 들어, 서버와 클라이언트 간의 연결입니다. Connection 객체는 데이터 전송과 수신을 관리하고, 네트워크의 상태를 모니터링하며, 필요한 경우 재연결 로직을 처리합니다.
* **Packet**: 
    * 네트워크를 통해 전송되는 데이터의 단위입니다. 
    * Packet은 정보를 포함하고 있으며, 데이터가 손실되거나 순서가 바뀌지 않도록 관리하는 메타데이터도 포함할 수 있습니다. Unreal Engine에서는 네트워크 패킷을 생성하고 해석하는 데 사용되는 여러 메커니즘을 제공합니다.
* **Channel**: 
    * Channel은 특정 유형의 데이터 또는 통신을 위한 가상의 경로입니다. 
    * 예를 들어, 하나의 채널은 게임 플레이 데이터를 처리할 수 있고, 다른 채널은 채팅 메시지를 처리할 수 있습니다. Channel을 사용하면 다양한 유형의 데이터를 분리하고 관리하는 데 도움이 됩니다.
* **Bunch**: 
    * Bunch는 하나 이상의 네트워크 메시지를 그룹화하는 데 사용됩니다. 
    * 이는 네트워크 효율성을 최적화하고 여러 메시지를 하나의 패킷으로 묶어 전송할 때 사용됩니다. Bunch를 통해 개발자는 데이터 전송을 더 효과적으로 제어할 수 있습니다.

```cpp
// 예를 들어, 서버에서 클라이언트로 게임 상태 업데이트를 보낼 때
bool UPacketHandlerComponent::SendGameUpdate()
{
    // FOutBunch는 Unreal의 네트워크 레이어에서 데이터를 묶는 데 사용되는 클래스입니다.
    FOutBunch Bunch(PlayerController->ClientConnection, false);

    // 메시지가 시작되었음을 표시합니다.
    Bunch.bOpen = true;

    // 메시지의 끝을 표시합니다.
    Bunch.bClose = true;

    // 여기에 게임 상태 데이터를 Bunch에 추가합니다.
    // 예를 들어, Player의 위치, 상태 등의 정보를 업데이트합니다.
    FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();
    Bunch << PlayerLocation;

    // 메시지를 보냅니다.
    PlayerController->ClientConnection->SendBunch(&Bunch, false);

    return Bunch.IsError();
}
```

---

## Actor를 Init하는 함수?

* 객체의 Init에는 보통 세 함수가 사용된다.
    * `PostInitializeComponents`, `PostNetInit`, `BeginPlay`

* **PostInitializeComponents**:
    * 이 이벤트는 객체가 생성되고 그 컴포넌트들이 초기화된 직후에 호출됩니다.
    * 이 단계는 객체의 기본 설정이 완료되었으며, 모든 컴포넌트들이 로드되고 초기 설정이 이루어진 시점에 실행됩니다.
    * 일반적으로 객체와 그것의 컴포넌트들에 대한 추가적인 설정이나 초기화를 수행하는 데 사용됩니다.
* **PostNetInit**:
    * 객체가 네트워크 상에서 초기화된 후에 호출됩니다.
    * 이는 주로 멀티플레이어 게임에서 사용되며, 네트워크로 연결된 객체가 올바르게 동기화되고 초기화되었음을 보장하는 데 중요합니다.
    * 객체가 **네트워크에 연결된 후** 필요한 추가적인 네트워크 설정이나 초기화를 수행하는 데 사용될 수 있습니다.
* **BeginPlay**:
    * 이 이벤트는 게임의 실제 플레이가 시작되기 바로 전에 호출됩니다.
    * 이 시점에서 모든 초기화 작업이 완료되었으며, 게임 오브젝트가 게임 월드에서 활동을 시작할 준비가 되었음을 의미합니다.
    * 이는 게임 로직을 시작하거나, 타이머를 설정하거나, 플레이어의 입력을 받기 시작하는 등 **게임 플레이와 관련된 작업**을 시작하기에 이상적인 시점입니다.

---

### Examples

```cpp
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	// ...
	
protected:
    // Login이후에 호출됨.
	virtual void PostInitializeComponents() override;

    // (주의) 네트워크에 연결된 후 호출되기에 오직 클라이언트에서만 호출됨.
	virtual void PostNetInit() override;
```

---

## Network 통신을 담당하는 클래스

* `PlayerController` - Network 통신에 접근하기 위해서 Controller를 통해 접근
    * `UNetConnection` - 데이터 패킷 인코딩, 디코딩, 통신량 조절, 채널 관리 등
    * `UNetDriver` - 로우레벨 패킷 처리, 통신 설정

---

### Examples

```cpp
void AABPlayerController::PostNetInit()
{
	Super::PostNetInit();

	UNetDriver* NetDriver = GetNetDriver();
	if (NetDriver)
	{
		if (NetDriver->ServerConnection)
		{
			// ...
		}
	}
	else
	{
		// ...
	}

}
```

---

## Ownership

* **PossessedBy**:
    * PossessedBy 함수는 컨트롤러가 어떤 액터(주로 캐릭터)를 소유하게 될 때 호출됩니다. 
    * 이 함수는 컨트롤러가 캐릭터를 "소유"하기 시작했음을 알리고, 해당 캐릭터에 필요한 초기 설정을 수행할 수 있는 기회를 제공합니다.
* **OnRep_Owner**:
    * OnRep_Owner 함수는 액터의 Owner 프로퍼티가 네트워크를 통해 복제될 때 호출됩니다. 
    * 이는 주로 멀티플레이어 게임에서, 액터의 **소유권이 변경**되었을 때 다른 클라이언트에게 그 사실을 알리고, 필요한 업데이트를 수행할 때 사용됩니다.
* **OnPossess**:
    * OnPossess 함수는 AI 컨트롤러가 캐릭터를 소유하게 될 때 호출됩니다. 
    * 이 함수는 AI가 새로운 캐릭터를 제어하기 시작할 때 초기화 작업을 수행할 수 있는 기회를 제공합니다.

