---
layout: default
title: "02-05. CreateNamedNetDriver"
parent: "([Network] 02. ServerListen)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 여기는 네트워크 드라이버를 찾거나 생성하는 중임을 잊지말자!
* (주의) NetDriver를 World에서 관리하지 않고 Engine에서 관리하는이유?
    * 중앙 집중식 관리
        * Engine은 모든 World 인스턴스들을 관리하는 중앙 컨트롤러 역할을 합니다
        * NetDriver는 여러 World에서 공유될 수 있는 리소스이므로, Engine 레벨에서 관리하는 것이 더 효율적입니다
    * 생명주기 관리
        * Engine의 생명주기는 World보다 깁니다
        * NetDriver는 World가 소멸되더라도 유지되어야 할 수 있으므로, Engine에서 관리하는 것이 안전합니다
    * 구성 관리
        * NetDriver의 설정은 BaseEngine.ini와 같은 엔진 레벨의 설정 파일에서 정의됩니다
        * Engine이 이러한 설정을 직접 관리하고 적용하는 것이 더 자연스럽습니다
    * 유연성
        * 하나의 Engine이 여러 World 인스턴스에 대한 NetDriver를 관리할 수 있습니다
        * 이를 통해 멀티플레이어 게임에서 여러 월드/레벨 간의 네트워크 통신을 더 유연하게 처리할 수 있습니다
        * 이러한 구조는 언리얼 엔진의 계층적 설계 철학과도 일치하며, 네트워크 기능의 중앙 집중식 관리를 가능하게 합니다.

* 예시?

1. 멀티플레이어 게임의 레벨 전환 시나리오

```
[상황]
- 4명의 플레이어가 있는 멀티플레이어 게임
- Map1 에서 Map2로 전환하는 상황
```

* 만약 World가 NetDriver를 관리한다면:

```
World(Map1) ─── NetDriver
    └─ 맵 전환 시 World 파괴
    └─ NetDriver도 함께 파괴
    └─ 모든 플레이어 연결 끊김 😱
```

* Engine이 NetDriver를 관리할 때:

```
Engine
  ├─ NetDriver (유지됨)
  │   └─ 플레이어 연결 유지 ✨
  │
  ├─ World(Map1) → 파괴
  └─ World(Map2) → 생성
```

2. 여러 월드가 동시에 존재하는 시나리오

```
[상황]
- 로비 월드: 플레이어들이 대기
- 게임 월드: 실제 게임 진행
- 관전 월드: 다른 게임을 구경
```

* Engine 관리의 장점:

```
Engine
  ├─ MainNetDriver
  │   └─ 모든 월드 간의 기본 통신 처리
  │
  ├─ World(로비)
  ├─ World(게임1)
  └─ World(관전)
```

* 이렇게 하면:
    * 하나의 NetDriver로 여러 월드의 통신을 효율적으로 관리
    * 월드가 변경되어도 플레이어 연결이 안정적으로 유지
    * 네트워크 설정을 중앙에서 일관되게 관리 가능
* 이것이 바로 Engine이 NetDriver를 관리하는 것이 더 효율적이고 안정적인 이유입니다.

```cpp
/** create a UNetDriver and associates a name with it */
bool CreateNamedNetDriver(UWorld *InWorld, FName NetDriverName, FName NetDriverDefinition)
{
    // see CreateNamedNetDriver_Local
    // - note that second parameter is 'NetDriverName' and third parameter is 'NetDriverDefinition', which we'll cover it soon
    return CreateNamedNetDriver_Local(this, GetWorldContextFromWorldChecked(InWorld), NetDriverName, NetDriverDefinition);
}
```

```cpp
bool CreateNamedNetDriver_Local(UEngine* Engine, FWorldContext& Context, FName NetDriverName, FName NetDriverDefinition)
{
    // we first try to find any existing net-driver
    //   - NetDriverName is "GameNetDriver"
        // 내부는 간단, 관리중인 Drivers가운데 이름을 찾아 리턴
    UNetDriver* NetDriver = FindNamedNetDriver_Local(Context.ActiveNetDrivers, NetDriverName);

    // in our case, our NetDriver is nullptr, so need to create new one
    // - see CreateNetDriver_Local
    if (NetDriver == nullptr)
    {
        NetDriver = CreateNetDriver_Local(Engine, Context, NetDriverDefinition, NetDriverName);
        if (NetDriver)
        {
            return true;
        }
    }
    return false;
}
```

```cpp
UNetDriver* CreateNetDriver_Local(UEngine* Engine, FWorldContext& Context, FName NetDriverDefinition, FName InNetDriverName)
{
    UNetDriver* ReturnVal = nullptr;
    FNetDriverDefinition* Definition = nullptr;

    auto FindNetDriverDefPred = 
        [NetDriverDefinition](const FNetDriverDefinition& CurDef)
    {
        return CurDef.DefName == NetDriverDefinition;
    };
    Definition = Engine->NetDriverDefinitions.FindByPredicate(FindNetDriverDefPred);

    // 'Definition' has following values:
    // - Definition.DefName == "GameNetDriver"
    // - Definition.DriverClassName == "/Script/OnlineSubsystemUtils.IpNetDriver"
    //   - you can find the value of DriverClassName in BaseEngine.ini

    // 'Definition'은 다음과 같은 값을 가집니다:
    // - Definition.DefName == "GameNetDriver"
    // - Definition.DriverClassName == "/Script/OnlineSubsystemUtils.IpNetDriver"
    //   - DriverClassName의 값은 BaseEngine.ini에서 찾을 수 있습니다
    
    // if we failed to find the definition, we CANNOT create net-driver!
    // 정의를 찾는 데 실패하면 net-driver를 생성할 수 없습니다!
    if (Definition != nullptr)
    {
        // NetDriverClass is returned as 'IpNetDriver' class
        // NetDriverClass는 'IpNetDriver' 클래스로 반환됩니다
        UClass* NetDriverClass = StaticLoadClass(UNetDriver::StaticClass(), nullptr, *Definition->DriverClassName.ToString(), nullptr, LOAD_Quiet);
        if (NetDriverClass != nullptr)
        {
            ReturnVal = NewObject<UNetDriver>(GetTransientPackage(), NetDriverClass);
            check(ReturnVal);

            // we set newly created net-driver as 'GameNetDriver'
            // - we add new ip-net-driver into ActiveNetDrivers
            
            // 새로 생성된 net-driver를 'GameNetDriver'로 설정합니다
            // - 새로운 ip-net-driver를 ActiveNetDrivers에 추가합니다
            const FName DriverName = InNetDriverName.IsNone() ? ReturnVal->GetName() : InNetDriverName;
            ReturnVal->SetNetDriverName(DriverName);
            ReturnVal->SetNetDriverDefinition(NetDriverDefinition);
            new (Context.ActiveNetDrivers) FNamedNetDriver(ReturnVal, Definition);
        }
    }

    return ReturnVal;
}
```

* 여기까지 마치면 NetDriver까지 할당 완료!
