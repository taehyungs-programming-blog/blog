---
layout: default
title: "09. InitWorld"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(OpenSource👽)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## InitWorld

```cpp
void InitWorld(const FWorldInitializationValues IVS = FWorldInitializationValues())
{
    // ...

    // Subsystem Init - 아래 참조
    InitializeSubsystems();

    // 이 타이밍에서 OnPreWorldInitialization가 Broadcast된다.
    FWorldDelegates::OnPreWorldInitialization.Broadcast(this, IVS);

    // 참고) WorldSettings은 Level의 PersistentLevel에 세팅되어있음
    AWorldSettings* WorldSettings = GetWorldSettings();
    if (IVS.bInitializeScenes)
    {
        if (IVS.bCreatePhysicsScene)
        {
            // 여기서 PhysicsScene생성
            CreatePhysicsScene(WorldSettings);
        }

        bShouldSimulatePhysics = IVS.bShouldSimulatePhysics;

        // 여기서 Render Scene 생성
        bRequiresHitProxies = IVS.bRequiresHitProxies;
        GetRendererModule().AllocateScene(this, bRequiresHitProxies, IVS.bCreateFXSystem, GetFeatureLevel());
    }

    Levels.Empty(1);
    // Level에 PersistentLevel등록
    Levels.Add(PersistentLevel);

    // PersistentLevel에 OwningWorld 등록
    PersistentLevel->OwningWorld = this;
    PersistentLevel->bIsVisible = true;

    // ...

    FWorldDelegate::OnPostWorldInitialization.Broadcast(this, IVS);

    PersistentLevel->OnLevelLoaded();

    // SreamingManager에 PersistentLevel이 준비됐음을 알린다.(이건 이후에 다시 설명)
    IStreamingManager::Get().AddLevel(PersistentLevel);

    PostInitializeSubsystems();

    BroadcastLevelsChanged();

    // ...
```

---

## UWorld::InitializeSubsystems

```cpp
void InitializeSubsystems()
{
    // Collection의 Initialize이다.
    SubsystemCollection.Initialize(this);
        // FObjectSubsystemCollection<UWorldSubsystem> SubsystemCollection;
}
```

---

## FSubsystemCollectionBase::Initialize

```cpp
void Initialize(UObject* NewOuter)
{
    // 이미 초기화 되진 않았나
    if (Outer)
    {
        return;
    }

    Outer = NewOuter;
    
    if (ensure(BaseType) &&                 // BaseType은 있어야함.
        ensure(SubSystemMap.Num() == 0))    // SubSystem이 이미 있다는건 다른데서 초기화 된 상태
    {
        check(IsInGameThread());    // 게임 스레드인가

        // ...

        {
            // UWorldSubsystem을 상속받는 클래스를 모두 가져온다.
            TArray<UClass*> SubsystemClasses;
            GetDerivedClasses(BaseType, SubsystemClasses, true);

            for (UClass* SubsystemClass : SubsystemClasses)
            {
                // UWorldSubsystem을 Init
                AddAndInitializeSubsystem(SubsystemClass);
            }
        }

        // ...
```

---

## FSubsystemCollectionBase::AddAndInitializeSubsystem

```cpp
USubsystem* AddAndInitializeSubsystem(UClass* SubsystemClass)
{
    // 이미 존재하나 체크
    if (!SubsystemMap.Contains(SubsystemClass))
    {
        if (SubsystemClass && !SubsystemClass->HasAllClassFlags(CLASS_Abstract))
        {
            check(SubsystemClass->IsChildOf(BaseType));

            const USubsystem* CDO = SubsystemClass->GetDefaultObject<USubsystem>();
            // see USubsystem::ShouldCreateSubsystem (goto 32)
            if (CDO->ShouldCreateSubsystem(Outer))
            {
                // ...
```