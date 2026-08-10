---
layout: default
title: "04. Actor의 역할 & Connection HandShake"
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

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/4)

## Actor의 역할

* `GetLocalRole()`:
    * GetLocalRole() 함수는 액터의 로컬 역할을 반환합니다. 
    * 이 역할은 액터가 현재 실행되고 있는 컨텍스트(서버 또는 클라이언트)에 따라 달라집니다. 반환되는 값은 ROLE_Authority, ROLE_SimulatedProxy, ROLE_AutonomousProxy 등이 될 수 있으며, 이는 각각 서버, 원격 클라이언트, 로컬 클라이언트를 나타냅니다.
* `GetRemoteRole()`:
    * GetRemoteRole() 함수는 액터의 원격 역할을 반환합니다. 
    * 이는 네트워크 상대방이 보는 이 액터의 역할을 의미합니다. 예를 들어, 서버에서 실행되고 있는 액터의 원격 역할은 클라이언트에서 볼 때 어떻게 되는지를 나타냅니다.
* 역할(Role)에는 다음과 같은 종류가 있다.
    * **ROLE_Authority**:
        * ROLE_Authority는 액터가 **서버에서 실행**되고 있음을 나타냅니다. 
        * 이 역할을 가진 액터는 게임의 모든 중요한 결정과 계산을 담당합니다. 
        * 예를 들어, 물리적 충돌, 게임 상태 변경, 점수 계산 등을 처리합니다. 
        * ROLE_Authority는 게임 로직과 상태 관리에 중추적인 역할을 합니다.
    * **ROLE_AutonomousProxy**:
        * ROLE_AutonomousProxy는 **플레이어에 의해 제어되는 액터**의 로컬 복사본을 나타냅니다. 
        * 이는 주로 플레이어의 클라이언트에서 실행되며, **사용자의 입력을 직접 받아 처리**합니다. 
        * 예를 들어, 플레이어가 캐릭터를 움직이거나, 무언가를 상호작용하려 할 때 해당 액터는 ROLE_AutonomousProxy 역할을 가집니다. 
        * 이 역할은 사용자의 **입력에 빠르게 반응**하고, 예측 기법을 사용하여 네트워크 **지연을 최소화**하는 데 중점을 둡니다. 
        * 사용자의 입력은 먼저 로컬에서 처리되고, 그 결과가 서버에 전송되어 권위 있는 상태와 동기화됩니다.
    * **ROLE_SimulatedProxy**:
        * ROLE_SimulatedProxy는 서버에서 ROLE_Authority 액터의 상태를 기반으로 하는 **원격 클라이언트의 액터 복사본**입니다. 
        * 이 역할은 **다른 플레이어들이나 원격 객체들을 표현**할 때 사용됩니다. 
        * 예를 들어, 다른 플레이어의 캐릭터, 움직이는 차량, 동적 환경 객체 등이 이에 해당합니다. 
        * ROLE_SimulatedProxy는 서버로부터 상태 업데이트를 받고, 이를 바탕으로 로컬에서 액터를 시뮬레이션합니다. 이는 네트워크 효율성을 높이고, 서버와의 지연 시간 동안에도 부드러운 애니메이션과 움직임을 제공하기 위해 사용됩니다.

```cpp
void AMyActor::CheckRoles()
{
    // 로컬 역할 확인
    ENetRole LocalRole = GetLocalRole();
    FString LocalRoleString;
    
    switch (LocalRole)
    {
        case ROLE_Authority:
            LocalRoleString = TEXT("Authority (Server)");
            break;
        case ROLE_AutonomousProxy:
            LocalRoleString = TEXT("Autonomous Proxy (Player-Controlled)");
            break;
        case ROLE_SimulatedProxy:
            LocalRoleString = TEXT("Simulated Proxy (Remote)");
            break;
        default:
            LocalRoleString = TEXT("Unknown");
            break;
    }

    // 원격 역할 확인
    ENetRole RemoteRole = GetRemoteRole();
    FString RemoteRoleString;
    
    switch (RemoteRole)
    {
        case ROLE_Authority:
            RemoteRoleString = TEXT("Authority (Server)");
            break;
        case ROLE_AutonomousProxy:
            RemoteRoleString = TEXT("Autonomous Proxy (Player-Controlled)");
            break;
        case ROLE_SimulatedProxy:
            RemoteRoleString = TEXT("Simulated Proxy (Remote)");
            break;
        default:
            RemoteRoleString = TEXT("Unknown");
            break;
    }

    // 결과 출력
    UE_LOG(LogTemp, Log, TEXT("Local Role: %s, Remote Role: %s"), *LocalRoleString, *RemoteRoleString);
}

```

---

## Connection HandShake

