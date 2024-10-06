---
layout: default
title: "01-01. FSocketSubsystemModule"
parent: "([Network] 01.SocketSubsystemModule)"
grand_parent: "(UE SourceCode 분석 🤖)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```csharp
// .build.cs 에 Module을 연결 시
PublicDependencyModuleNames.AddRange(new string[] { "Sockets", "Networking" });
```

* `D:\UnrealEngine\Engine\Source\Runtime\Sockets\Public` 의 `SocketSubsystem.h` 의 아래 코드가 실행된다.

```cpp
/**
 * socket subsystem module class:
 * - wraps the loading of an socket subsystem by name and allows new services to register themselves for use
 */
class FSocketSubsystemModule : public IModuleInterface
{
    /** name of the default socket subsystem defined by the platform */
    FName DefaultSocketSubsystem;

    /** mapping of all currently loaded subsystems to their name */
    TMap<FName, class ISocketSubsystem*> SocketSubsystems;

    // 시작은 여기!
    virtual void StrartupModule() override
    {
        // initialize the platform defined socket subsystem first
            // 먼저 플랫폼에서 정의된 소켓 서브시스템을 초기화합니다.

        // if you are running in windows platform, call CreateSocketSubsystem() in SocketSubsystemWindows.cpp
        // e.g. if you are running in MAC, it will call CreateSocketSubsystem() in SocketSubsystemMac.cpp
            // Windows 플랫폼에서 실행 중이라면 SocketSubsystemWindows.cpp의 CreateSocketSubsystem()을 호출합니다.
            // 예를 들어, MAC에서 실행 중이라면 SocketSubsystemMac.cpp의 CreateSocketSubsystem()을 호출할 것입니다.

        DefaultSocketSubsystem = CreateSocketSubsystem(*this);
    }
```

```cpp
FName CreateSocketSubsystem(FSocketSubsystemModule& SocketSubsystemModule)
{
    // we are in SocketSubsystemWindows.h/.cpp
    // - our DefaultSocketSubsystem's FNAME will be 'WINDOWS'
        // 우리는 SocketSubsystemWindows.h/.cpp 파일 내에 있습니다
        // - 우리의 DefaultSocketSubsystem의 FNAME은 'WINDOWS'가 될 것입니다

    FName SubsystemName(TEXT("WINDOWS"));

    
    FSocketSubsystemWindows* SocketSubsystem = FSocketSubsystemWindows::Create();


    // FSocketSubsystemWindows 함수 관련하여는 다음장에서 설명.
    FString Error;
    if (SocketSubsystem->Init(Error))
    {
        // see briefly FSocketSubsystemModule::RegisterSocketSubsystem
        // - nothing special, just manage created socket-subsystems
            // FSocketSubsystemModule::RegisterSocketSubsystem 참조 (1)
            // - 특별한 것은 없고, 단지 생성된 소켓 서브시스템들을 관리합니다

        SocketSubsystemModule.RegisterSocketSubsystem(SubsystemName, SocketSubsystem);
        return SubsystemName;
    }

    // 실패! 소켓을 닫자
    FSocketSubsystemWindows::Destroy();
    return NAME_None;
}
```


#### (1) FSocketSubsystemModule::RegisterSocketSubsystem

```cpp
/** register a new socket subsystem interface with the base level factory provider */
virtual void RegisterSocketSubsystem(const FName FactoryName, class ISocketSubsystem* Factory, bool bMakeDefault=false)
{
    if (!SocketSubsystems.Contains(FactoryName))
    {
        SocketSubsystems.Add(FactoryName, Factory);
    }

    if (bMakeDefault)
    {
        DefaultSocketSubsystem = FactoryName;
    }
}
```
