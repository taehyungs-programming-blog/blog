---
layout: default
title: "03. CreateWorld"
parent: "(Engine Source Code 분석 1)"
grand_parent: "(OpenSource👽)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Engine Init

* 시작은 Engine의 Init시점이다.

```cpp
// EditorEngine.cpp

virtual void Init(IEngineLoop* InEngineLoop)
{
    // ...

    if (GIsEditor)
	{
		// ...

		// InitialWorldContext에 UWorld::CreateWorld를 통하여
		// World를 만들어 넣게된다.
		InitialWorldContext.SetCurrentWorld( UWorld::CreateWorld( EWorldType::Editor, true ) );
		GWorld = InitialWorldContext.World();
	}
}
```

```cpp
static UWorld* CreateWorld(/* ... */)
{
	// 여기서 UPackage라는 Package는 File로 저장했다 불러왔다 하는 Object라 생각하자
	UPackage* WorldPackage = InWorldPackage;
	if (!WorldPackage)
	{
		// 더미 Package를 만들어 둔다.
		WorldPackage = CreatePackage(nullptr);
	}

	// ...

	// Transient라는 개념이 나오는데 Transient란
		// Object가 직렬화 될 것인가? 여부를 체크한다.
	if (WorldPackage != GetTransientPackage())
	{
		WorldPackage->ThisContainsMap();
	}

	// ...

	// 이런 과정을 통해 UWorld가 생성되고 UWorld는 다음 강읠 참조 하자.
	UWorld* NewWorld = NewObject<UWorld>(WorldPackage, *WorldNameString);

	// ...
	
	// 여기는 내용이 길어서 8강 참조!
	NewWorld->InitializeNewWorld(
}
```

---

