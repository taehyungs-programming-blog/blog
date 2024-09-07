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

    virtual void StratupModule() override
    {
        // initialize the platform defined socket subsystem first
        // if you are running in windows platform, call CreateSocketSubsystem() in SocketSubsystemWindows.cpp
        // e.g. if you are running in MAC, it will call CreateSocketSubsystem() in SocketSubsystemMac.cpp
        //
        // how does it work?
        // - I didn't try to find how to redirect CreateSocketSubsystem() in different platform
        // - in the past, UE3 use the trick that in windows platform, only include SocketSubsystemWindows.h/.cpp 
        //   and for Mac, SocketSubsystemMac.h/.cpp depending on compiler option using platform's MACRO (e.g. PLATFORM_WINDOWS)
        // - I assume that it works in similar manner:
        //   - **when you set BP in SocketSubsystemMac.h/.cpp, we'll notice that it FAILs to hit bp cuz missing PDB**
        //     - from this observation, we can see the files(SocketSubsystemMac.h/.cpp) are not included to compile!

        DefaultSocketSubsystem = CreateSocketSubsystem(*this);
    }
```

```cpp
FName CreateSocketSubsystem(FSocketSubsystemModule& SocketSubsystemModule)
{
    // we are in SocketSubsystemWindows.h/.cpp
    // - our DefaultSocketSubsystem's FNAME will be 'WINDOWS'
    FName SubsystemName(TEXT("WINDOWS"));

    // create and register our singleton factory with the main online subsystem for easy access
    // first we are looking into FSocketSubsystemWindows
    FSocketSubsystemWindows* SocketSubsystem = FSocketSubsystemWindows::Create();

    // now initialize SocketSubsystem, 
    FString Error;
    if (SocketSubsystem->Init(Error))
    {
        // see briefly FSocketSubsystemModule::RegisterSocketSubsystem
        // - nothing special, just manage created socket-subsystems
        SocketSubsystemModule.RegisterSocketSubsystem(SubsystemName, SocketSubsystem);
        return SubsystemName;
    }

    // if we reach here, it means we failed to initialize SocketSubsystemWindows
    // - see briefly SocketSubsystemWindows::Destroy
    FSocketSubsystemWindows::Destroy();
    return NAME_None;
}
```