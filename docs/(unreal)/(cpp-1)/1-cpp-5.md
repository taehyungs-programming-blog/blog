---
layout: default
title: "05. 체력바"
parent: "(Unreal C++ RPG Action만 구현)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 모듈의 설치가 필요하다

```csharp
using UnrealBuildTool;

public class ProjectRPG : ModuleRules
{
	public ProjectRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule", "GameplayTasks", "NavigationSystem", 

        // UMG모듈 필요
        "UMG" });
	}
}
```

<사진>

* 우클릭 후 UI -> 사용자 위젯 생성

<사진2>

* 이제 C++클래스로 상속해 사용해 보자

```cpp
UCLASS()
class PROJECTRPG_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UProgressBar* HealthBar;

	void SetHealthBarValue(float Value);
};
```

* Health Bar의 사용은

```cpp
UCLASS(config=Game)
class AProjectRPGCharacter : public ACharacter
{
    // ...
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY()
	class UWidgetComponent* HealthBarWidget;

public:
	float GetMaxHealth();

	void SetHealthBar();

    // ...
```

```cpp
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	
	HealthBarWidget->SetupAttachment(RootComponent);
	
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/ProjectRPG/UI/WBP_Health"));
	
	if (WidgetClass.Succeeded())
	{
		HealthBarWidget->SetWidgetClass(WidgetClass.Class);
	}

```