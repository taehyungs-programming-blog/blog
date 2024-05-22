---
layout: default
title: "26. ui - 1"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/3)

---

## Widget BluePrint를 하나 만들어 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-26-1.png"/>
</p>

* 흠... 그런데 이걸 실제 예시로 보는게 더 좋을 듯 하다.

---

## 실사용 예시

### 캐릭터 머리위의 HP Bar 구현

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-26-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-26-3.png"/>
</p>

* Override가 가능하게 만들기

```cpp
AR1Character::AR1Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HpBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HpBarComponent->SetupAttachment(GetRootComponent());

	ConstructorHelpers::FClassFinder<UUserWidget> HealthBarWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_HpBar.WBP_HpBar_C'"));
	if (HealthBarWidgetClass.Succeeded())
	{
		HpBarComponent->SetWidgetClass(HealthBarWidgetClass.Class);
		HpBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
		HpBarComponent->SetDrawAtDesiredSize(true);
		HpBarComponent->SetRelativeLocation(FVector(0, 0, 100));
	}
}
```

```cpp
UCLASS()
class R1_API UR1HpBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UR1HpBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;

public:
	void SetHpRatio(float Ratio);

protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	TObjectPtr<class UProgressBar> HpBar;
};

```

```cpp
UR1HpBarWidget::UR1HpBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UR1HpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UR1HpBarWidget::SetHpRatio(float Ratio)
{
	if (HpBar)
	{
		HpBar->SetPercent(Ratio);
	}
}
```