---
layout: default
title: "09. 캐릭터 스탯과 위젯"
parent: "(Unreal C++ Part2 - 게임만들어 보기)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Cpp_Basic_2/tree/8)

## 캐릭터 스탯 설정

* 새로운 C++ 클래스가 필요하다
* Tools -> New C++ Class -> ActorComponent
    * ABCharacterStatComponent

* ABCharacterStatComponent를 구현하기 전 Hp바를 먼저 만들어 보자
* User Interface -> Widget Blueprint
    * Vertical Box아래 Progress Bar를 하나 추가

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_9_1.png"/>
</p>

* Hp바를 상속할 C++ 클래스를 만들어보자
* Tools -> New C++ Class -> UserWidget
* 생성후 WBP_HpBar의 Parent를 위에서 생성한 C++(ABHpBar)로 변경하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_2/ucpp_2_9_2.png"/>
</p>

```cpp
// 캐릭터 스탯부터 만들어 보자

// Delegate선언 참조
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

	FORCEINLINE float GetMaxHp() { return MaxHp; }
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	float ApplyDamage(float InDamage);

protected:
	void SetHp(float NewHp);

	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;
		
};
```

```cpp
// ...

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
        // Broadcast에 주목
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
    
    // Broadcast에 주목
	OnHpChanged.Broadcast(CurrentHp);
}
```

* 자! 이제 Delegate를 어떻게 받아 처리할지 보자

```cpp
UCLASS()
class ARENABATTLE_API UABHpBarWidget : public UABUserWidget
{
	GENERATED_BODY()
	
public:
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateHpBar(float NewCurrentHp);

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
```

```cpp
// UI 관련 정보가 초기화가 끝난 상태에서 호출이 된다
void UABHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);

	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}
```

* CharacterBase에도 UI Section을 만들자

```cpp
UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABAnimationAttackInterface, public IABCharacterWidgetInterface
{
	GENERATED_BODY()

    //...

    // Stat Section
    protected:
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
        TObjectPtr<class UABCharacterStatComponent> Stat;

    // UI Widget Section
    protected:
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
        TObjectPtr<class UWidgetComponent> HpBar;

        virtual void SetupCharacterWidget(class UABUserWidget* InUserWidget) override;
```

```cpp
// 초기화는 이렇게

// Stat Component 
Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));

// Widget Component 
HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));
HpBar->SetupAttachment(GetMesh());
HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C"));
if (HpBarWidgetRef.Class)
{
    HpBar->SetWidgetClass(HpBarWidgetRef.Class);
    HpBar->SetWidgetSpace(EWidgetSpace::Screen);
    HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
    HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
```

```cpp
void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	UABHpBarWidget* HpBarWidget = Cast<UABHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);
	}
}
```

---

* 잠시! 구현중 조금 특이한 부분이 있는데

```cpp
// ???
    // UABHpBarWidget가 UABUserWidget를 상속하게 만들었네?
    // UABUserWidget는 내가 만든 클래스이다.
class ARENABATTLE_API UABHpBarWidget : public UABUserWidget
```

```cpp
// 얘를 왜 넣었냐?
    // RunTime에 HpBar가 Owner를 알길이 없음.
UCLASS()
class ARENABATTLE_API UABUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetOwningActor(AActor* NewOwner) { OwningActor = NewOwner; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor")
	TObjectPtr<AActor> OwningActor;
};
```

---

