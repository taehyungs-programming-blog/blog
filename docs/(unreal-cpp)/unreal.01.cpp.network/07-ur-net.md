---
layout: default
title: "07. Actor Replication (Low-level-Flow)"
parent: "(Network)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/7)

## Actor Replication Priority

* 이 시스템은 어떤 액터(게임 내 객체)가 네트워크를 통해 다른 플레이어에게 **얼마나 자주, 그리고 얼마나 중요하게 업데이트될지를 결정**합니다. 
* 각 액터에는 `Replication Priority`라는 값이 있으며, 이 값이 높을수록 더 자주 및 우선적으로 업데이트됩니다.

* 액터 복제 우선 순위 설정:
    * 액터의 복제 우선 순위는 다음과 같은 요소에 따라 달라질 수 있습니다:
    * **거리**: 플레이어로부터의 거리가 가까운 액터는 더 높은 우선 순위를 가질 수 있습니다.
    * **시각적 중요성**: 화면에 보이는 액터나 중요한 게임 플레이 요소는 더 높은 우선 순위를 받을 수 있습니다.
    * **게임 플레이 중요성**: 특정 게임 플레이 메커니즘에 중요한 액터는 더 빈번하게 업데이트되어야 할 수 있습니다.

```cpp
void AMyActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 복제 우선 순위 설정
    DOREPLIFETIME_CONDITION(AMyActor, MyImportantVariable, COND_SkipOwner);
    // 이 코드는 'MyImportantVariable'이라는 변수가 복제될 때 특정 조건(여기서는 소유자를 제외하고)을 적용합니다.
}

void AMyActor::PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker)
{
    Super::PreReplication(ChangedPropertyTracker);

    // 복제 우선 순위를 동적으로 조정
    SetNetPriority(CalculateMyCustomPriority());
}

float AMyActor::CalculateMyCustomPriority()
{
    // 우선 순위 계산 로직
    // 예: 플레이어와의 거리, 중요한 게임 이벤트 발생 등을 기반으로 한 계산
    return CustomPriorityValue;
}

```

---

## 테스트 해보기

```ini
; DefaultEngine.ini에서 Bandwidth를 작게 설정(기본은 32000)
[/Script/Engine.GameNetworkManager]
TotalNetBandwidth=3200
```

---

## Actor Dormancy(휴먼)

* 네트워크 최적화를 위해 사용되는 기능으로, 특정 액터가 일정 기간 동안 **네트워크 업데이트를 중지**하도록 설정할 수 있습니다. 
* 이는 네트워크 대역폭을 절약하고 성능을 향상시키기 위해 사용됩니다. 액터가 '쉬고(dormant)' 있는 동안, 그 상태는 변경되지 않는다고 가정하며, 따라서 서버는 해당 액터에 대한 업데이트를 클라이언트에게 보내지 않습니다. 
* 액터의 상태가 다시 변경되면, 액터는 '활성(active)' 상태로 전환되고 네트워크 업데이트가 재개됩니다.

* Dormancy 유형:
    * `DORM_Never`: 액터는 절대 쉬지 않음 (기본 설정).
    * `DORM_Awake`: 액터가 현재 활성 상태이지만, 조건이 충족되면 잠자는 상태로 전환될 수 있음.
    * `DORM_DormantAll`: 액터가 모든 클라이언트에 대해 쉬고 있음.
    * `DORM_DormantPartial`: 액터가 일부 클라이언트에 대해서만 쉬고 있음 (특정 조건에 따라 다름).
    * `DORM_Initial`: 액터를 휴먼상태로 시작하고 필요한 때 깨우도록 설정

```cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 이 액터를 쉬게 설정. 변경사항이 없을 때 네트워크 업데이트를 중지합니다.
    SetNetDormancy(ENetDormancy::DORM_DormantAll);

    // 여기에 초기화 코드나 다른 로직을 추가할 수 있습니다.
}

void AMyActor::OnSomeImportantEvent()
{
    // 중요한 이벤트가 발생했을 때 액터를 깨웁니다.
    // 이는 네트워크 업데이트를 다시 시작하게 합니다.
    ForceNetUpdate();
    SetNetDormancy(ENetDormancy::DORM_Awake);
}

```
## 알아두면 좋을 클래스

* **FNetworkObjectInfo**
    * 네트워크에서 액터의 상태를 추적하는 구조체입니다. 이 구조체는 액터의 리플리케이션과 관련된 다양한 정보를 포함하며, **서버에서 각 액터의 리플리케이션을 관리**하는 데 사용됩니다.
    * 예를 들어, 액터의 마지막 리플리케이션 시간, 액터의 네트워크 우선 순위, 액터의 상태 변경 여부 등의 정보가 여기에 포함될 수 있습니다. 이를 통해 Unreal Engine은 **어떤 액터를 언제 리플리케이션할지 결정하는 데 필요한 정보를 갖게 됩니다.**
* **ConsiderList**
    * 리플리케이션을 고려해야 하는 **액터들의 목록**을 의미합니다. 이 리스트는 특정 시점에서 서버가 클라이언트에게 상태를 업데이트해야 할 가능성이 있는 모든 액터를 포함합니다.
    * 각 리플리케이션 사이클마다, Unreal Engine은 이 목록을 검토하여 어떤 액터들이 실제로 데이터를 보낼 자격이 있는지 결정합니다. 액터의 중요성, 거리, 가시성 등 여러 요소가 이 결정에 영향을 미칠 수 있습니다.
* **PriorityList**
    * 리플리케이션을 위해 선택된 액터들의 정렬된 목록입니다. 이 목록은 각 액터의 중요도나 우선 순위에 따라 정렬되며, 가장 중요한 액터부터 리플리케이션됩니다.
    * 네트워크 대역폭은 제한적이므로, 모든 액터를 한 번에 리플리케이션할 수 없습니다. PriorityList는 가장 중요한 액터부터 처리함으로써, 제한된 리소스를 가장 효과적으로 사용할 수 있도록 돕습니다. 액터의 우선 순위는 거리, 활동 수준, 게임에 대한 영향력 등 다양한 요소에 따라 달라질 수 있습니다.

    