---
layout: default
title: "09. 캐릭터 공격 구현"
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

## Ver 1.0

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/9)

* Player에서 Attack을 한다고 가정

```cpp
void AABCharacterPlayer::Attack()
{
	if (bCanAttack)
	{
		ServerRPCAttack();
        /*
        UFUNCTION(Server, Reliable, WithValidation)
        void ServerRPCAttack();
        */
	}
}
```

* 복습: `ServerRPCAttack`에 관하여

* `Server`: 
    * 이 속성은 **함수가 서버에서 실행**되도록 지정합니다. 
    * 클라이언트가 이 함수를 호출하면, 호출은 서버로 전달되어 서버에서 실행됩니다. 이는 주로 게임의 상태를 변경하거나 중요한 계산을 수행할 때 사용되며, 서버가 게임의 권위자로서 모든 중요한 결정을 내리도록 보장합니다.
* `Reliable`: 
    * 이 속성은 네트워크를 통해 함수 호출이 신뢰성 있게 전송되어야 함을 의미합니다. 
    * 즉, 이 메시지는 네트워크 문제나 지연이 있더라도 반드시 도착하고 실행되어야 합니다. 이는 중요한 함수 호출에 사용됩니다, 예를 들어, 플레이어의 생명력 감소, 점수 증가 또는 게임의 상태 변화와 같은 중요한 이벤트를 처리할 때 사용됩니다.
* `WithValidation`: 
    * 이 속성은 서버가 실행하기 전에 함수 호출을 검증할 수 있는 추가적인 코드를 작성할 수 있도록 합니다. 
    * 검증 함수는 클라이언트가 보낸 데이터가 유효하고, 안전하며, 예상대로 동작하는지 확인하는 데 사용됩니다. 이는 부정 행위 방지나 잘못된 데이터로 인한 오류를 방지하는 데 도움이 됩니다.

```cpp
// 서버에서 아래 두 함수가 호출됨.

bool AABCharacterPlayer::ServerRPCAttack_Validate()
{
	// ...
}

void AABCharacterPlayer::ServerRPCAttack_Implementation()
{
	// ...
    MulticastRPCAttack();
}
```

---

```cpp
UFUNCTION(NetMulticast, Reliable)
void MulticastRPCAttack();
```

* `NetMulticast`: 
    * 이 속성은 함수가 '멀티캐스트' RPC임을 나타냅니다. 멀티캐스트 RPC는 서버에서 한 번 호출되며, 그 다음 모든 클라이언트(또는 일부 조건에 따라 필터링된 클라이언트)에서 실행됩니다. 이는 일반적으로 모든 플레이어에게 보이는 효과나 애니메이션, 환경 변화 등을 동기화할 때 사용됩니다.

```cpp
void AABCharacterPlayer::MulticastRPCAttack_Implementation()
{
/*
* HasAuthority()의미
    * 서버에서 호출될 때: 
        * 서버가 HasAuthority()를 호출하면, 이 함수는 true를 반환합니다. 
        * 즉, 이 객체(예: 캐릭터, 아이템 등)가 서버에서 실행되고 있으며, 게임 로직이나 상태를 변경할 권한이 있다는 의미입니다.

    * 클라이언트에서 호출될 때: 
        * 클라이언트가 HasAuthority()를 호출하면, 대부분의 경우 false를 반환합니다. 
        * 이는 클라이언트가 객체의 주요 상태를 직접 변경할 권한이 없음을 나타냅니다.
*/
	if (HasAuthority())
	{
		bCanAttack = false;
		OnRep_CanAttack();

		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				bCanAttack = true;				
				OnRep_CanAttack();
			}
		), AttackTime, false, -1.0f);

	}

    // Attack Animation 출력
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(ComboActionMontage);
}
```

* `GetLifetimeReplicatedProps`
    * 이 함수는 객체의 속성들이 네트워크를 통해 어떻게 복제(replicate)될지 결정하는 데 사용됩니다. 
    * 복제는 네트워크 상의 다른 인스턴스(예: 다른 플레이어의 게임 클라이언트)와 게임 데이터를 동기화하는 과정입니다. 
    * 멀티플레이어 게임에서는 플레이어의 위치, 상태, 점수 등 중요한 정보가 모든 클라이언트에 실시간으로 반영되어야 하므로, 이러한 정보의 복제는 게임의 일관성과 실시간 반응성을 유지하는 데 필수적입니다.

```cpp
/*
UPROPERTY(ReplicatedUsing = OnRep_CurrentHp, Transient, VisibleInstanceOnly, Category = Stat)
float CurrentHp;
*/
void UABCharacterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UABCharacterStatComponent, CurrentHp);
}
```

---

## Ver 2.0 (개선버전)

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/10)

* 클라이언트의 모든 행동이 서버를 거친 후 수행되기에, 통신부하가 발생시 어떠한 문제가 발생할 것이다.
* 우선 임의의 패킷랙을 만들어 보자

```ini
; DefaultEngine.ini

[PacketSimulationSettings]
PktLag=500
```

* 될수있으면 클라이언트에서 계산을 해야할 부분은 클라에서 해야한다.

```cpp
UFUNCTION(Client, Unreliable)
void ClientRPCPlayAnimation(AABCharacterPlayer* CharacterToPlay);

UFUNCTION(Server, Reliable, WithValidation)
void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);

UFUNCTION(Server, Reliable, WithValidation)
void ServerRPCNotifyMiss(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime);
```

```cpp
bool AABCharacterPlayer::ServerRPCNotifyHit_Validate(const FHitResult& HitResult, float HitCheckTime)
{
	return (HitCheckTime - LastAttackStartTime) > AcceptMinCheckTime;
}

void AABCharacterPlayer::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult, float HitCheckTime)
{
	// ...
}

bool AABCharacterPlayer::ServerRPCNotifyMiss_Validate(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime)
{
	return (HitCheckTime - LastAttackStartTime) > AcceptMinCheckTime;
}


void AABCharacterPlayer::ServerRPCNotifyMiss_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir, float HitCheckTime)
{
	// ...
}
```