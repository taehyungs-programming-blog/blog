---
layout: default
title: "13. Build System"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_basic/tree/main/11.BuildSystem)

```json
{
	"FileVersion": 3,
	"EngineAssociation": "5.1",
	"Category": "",
	"Description": "",
	/*필요한 모듈을 지정한다*/
	"Modules": [
		{
			"Name": "UnrealBuildSystem",
			"Type": "Runtime",
			"LoadingPhase": "Default",
			"AdditionalDependencies": [
				"CoreUObject"
			]
		},
		/*이전시간에 작업한 모듈*/
		{
			"Name": "UnrealSerialization",
			"Type": "Runtime"
		}
	]
}
```

---

## 한 번 해보자

* unreal project file을 아래와 같이 만든다

```json
{
	"FileVersion": 3,
	"EngineAssociation": "5.1",
	"Modules": [
		{
			/*UnrealBuildSystem라는 Module을 만들어볼 예정*/
			"Name": "UnrealBuildSystem",
			"Type": "Runtime"
		}
	]
}
```

* `Source\UnrealBuildSystem`폴더 생성
* `Source\UnrealBuildSystemEditor.Target.cs` 생성

```csharp
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealBuildSystemEditorTarget : TargetRules
{
	public UnrealBuildSystemEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("UnrealBuildSystem");
	}
}
```

* `Source\UnrealBuildSystem\UnrealBuildSystem.Build.cs` 생성

```csharp
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealBuildSystem : ModuleRules
{
	public UnrealBuildSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		// Unreal이 제공하는 기본 모듈
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "CommonUtility" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
```

* `UnrealBuildSystem.h/cpp` 생성

```cpp
#include "UnrealBuildSystem.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, UnrealBuildSystem, "UnrealBuildSystem");
```

* project 파일을 우클릭 후 -> Visual Studio Project를 생성한다

---

## Plug-in

* 일종의 Sub-Module이라 생각하면 편하다

### 그냥 만들어 보자!

* `Plugins` 폴더생성
* `GameUtility.uplugin` 생성 후 json형식으로 아래 작성

```json
{
	"FileVersion": 3,
	"Version": 1,
	"VersionName": "1.0",
	"FriendlyName": "GameUtility", 
	"Modules": [
		{
			"Name": "CommonUtility",
			"Type": "Runtime",
			"LoadingPhase": "Default"
		}
	]
}
```

* 나머지는 Module과 유사하다
* `GameUtility\CommonUtility.Build.cs` 생성

```csharp
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CommonUtility : ModuleRules
{
	public CommonUtility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
```

* 특이한 점은 외부 노출 Class는 Public 폴더에
* 외부에 노출하지 않을 경우 Private 폴더에 코드를 생성