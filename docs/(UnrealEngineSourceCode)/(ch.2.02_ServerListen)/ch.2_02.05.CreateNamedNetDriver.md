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
