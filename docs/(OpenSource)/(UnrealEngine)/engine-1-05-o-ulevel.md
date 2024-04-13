---
layout: default
title: "05. ULevel"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(OpenSource👽)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## ULevel

```cpp
// level은 actor의 집합이라 생각하자
class ULevel : public UObject
{
    // ...

    // ULevel도 역시 멤버변수를 확인하자
    TArray<TObjectPtr<AActor>> Actors;
        // level은 actor의 집합이라 생각하자 Actor를 보자면 다음강 참조!
    FLevelCollection* CachedLevelCollection;
        // level의 Collection을 정의 아래를 참조하자

    // initilize시 증분적(순차적) 으로 State를 관리해 로드
        // 이렇게 안하면 모두 로드해야함.
    EIncrementalComponentState IncrementalComponentState;
}
```

---

## FLevelCollection

```cpp

struct FLevelCollection
{
    /*
    enum class ELevelCollectionType : uint8
    {
        DynamicSourceLevels,
        DynamicDuplicatedLevels,    // 동적인 level
        StaticLevels,               // static한 level(움직이지 않는)
        MAX
    };
    // 간단히 Dynamic과 Static의 분리용이다.
    */
    ELevelCollectionType CollectionType;

    // Actor는 Hierarchy 구조를 갖는다 했다.
    // Level도 마치 Hierarchy 구조처럼 구조를 만들기 위해 아래 처럼 변수를 사용
    TObjectPtr<class ULevel> PersistentLevel;

    TSet<TObjectPtr<ULevel>> Levels;
};
```

* 뭔소리지?
    * 그림으로 설명 

```
World 0 (OwningWorld) -- [OuterPrivate] -- Package (World.umap)
  |
  |
[OuterPrivate]
  |
  |
Level 0 (PersistentLevel)
  |
  ---- Level 1 -- [OuterPrivate] -- World1 -- [OuterPrivate] -- Package (Level1.umap)
  |
  ---- Level 2 -- World2 -- Package2 (Level2.umap)
```

```cpp
// (참고)
class UObjectBase
{
    // ...

    // 어떤 Package(File)에 저장되어 있는지 정보를 담는다
        // 주의할 점은 UE가 발전하며 항상 Package(File)이 아닐수 있다는 점이다.
    UObject* OuterPrivate;
```

* World 0(Level 0의 OwningWorld)의 OuterPrivate 즉 Package는 World.umap로 선언되어 있다.
* Level 0의 OuterPrivate는 World 0이다.
* Level 1, Level 2 ...
* PersistentLevel 을 Root라 생각하면 되겠다.
    * Root 내부에 Level 존재하고 각 Level은 자신의 OuterPrivate를 갖는 개념이다.

