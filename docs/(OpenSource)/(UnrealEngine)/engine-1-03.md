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

```cpp
// EditorEngine.cpp

virtual void Init(IEngineLoop* InEngineLoop)
{
    // ...

    // World Context를 만들고
    FWorldContext &EditorContext = GetEditorWorldContext();
    /*
FWorldContext& UEngine::CreateNewWorldContext(EWorldType::Type WorldType)
{
	FWorldContext* NewWorldContext = new FWorldContext;
	WorldList.Add(NewWorldContext);
	NewWorldContext->WorldType = WorldType;
	NewWorldContext->ContextHandle = FName(*FString::Printf(TEXT("Context_%d"), NextWorldContextHandle++));

	return *NewWorldContext;
}
    */

    // ...

    // Current World까지 만들면된다.
    if (GIsEditor)
	{
		// Create a WorldContext for the editor to use and create an initially empty world.
		FWorldContext &InitialWorldContext = CreateNewWorldContext(EWorldType::Editor);
		InitialWorldContext.SetCurrentWorld( UWorld::CreateWorld( EWorldType::Editor, true ) );
		GWorld = InitialWorldContext.World();
	}
}
```

---
