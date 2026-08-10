---
layout: default
title: "26. ui"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}
s
---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/4)

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

```cpp
void AR1Character::RefreshHpBarRatio()
{
	if (HpBarComponent)
	{
		float Ratio = static_cast<float>(Hp) / MaxHp;
		UR1HpBarWidget* HpBar = Cast<UR1HpBarWidget>(HpBarComponent->GetUserWidgetObject());
		HpBar->SetHpRatio(Ratio);
	}
}
```

```cpp
void AR1Character::OnDamaged(int32 Damage, TObjectPtr<AR1Character> Attacker)
{
	Hp = FMath::Clamp(Hp - Damage, 0, MaxHp);
	if (Hp == 0)
	{
		OnDead(Attacker);
	}

	RefreshHpBarRatio();
}
```

---

### Inventor 만들어보기

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-26-4.png"/>
</p>

```cpp
class UImage;
class USizeBox;

UCLASS()
class R1_API UR1InventorySlotWidget : public UR1UserWidget
{
	GENERATED_BODY()
	
public:
	UR1InventorySlotWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Slot;
};
```

```cpp
UCLASS()
class R1_API UR1InventorySlotsWidget : public UR1UserWidget
{
	GENERATED_BODY()
	

public:
	UR1InventorySlotsWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TSubclassOf<UR1InventorySlotWidget> SlotWidgetClass;

	UPROPERTY()
	TArray<TObjectPtr<UR1InventorySlotWidget>> SlotWidgets;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel_Slots;
};
```

---

* UI와 Data를 분리해보자

```cpp
UCLASS(BlueprintType)
class R1_API UR1ItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UR1ItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(int32 InItemID);
	
public:
	UPROPERTY()
	int32 ItemID = 0;

	UPROPERTY()
	EItemRarity ItemRarity = EItemRarity::Junk;
};
```

```cpp
void UR1InventorySubsystem::AddDefaultItems()
{
	TObjectPtr<UR1ItemInstance> Item = NewObject<UR1ItemInstance>();
	Item->Init(100);

	Items.Add(Item);
}
```

```cpp
void AR1GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UR1InventorySubsystem* Inventory = Cast<UR1InventorySubsystem>(USubsystemBlueprintLibrary::GetWorldSubsystem(this, UR1InventorySubsystem::StaticClass()));
	if (Inventory)
	{
		Inventory->AddDefaultItems();
	}
}
```

* `UR1InventorySubsystem` 이란?

```cpp
UCLASS()
class R1_API UR1InventorySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	// TEMP
	void AddDefaultItems();

	const TArray<TObjectPtr<UR1ItemInstance>>& GetItems() { return Items; }

protected:
	UPROPERTY()
	TArray<TObjectPtr<UR1ItemInstance>> Items;
};
```

* Subsystem이 뭐지?
    * [Docs 🌟](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/programming-subsystems-in-unreal-engine?application_version=5.3)
    *  게임의 전역적인 기능을 관리하는 구조를 제공하여, 특정 시스템이나 기능을 게임의 모든 부분에서 쉽게 접근하고 사용할 수 있게 해줍니다
    * 전역 접근성: SubSystem은 **전역적으로 접근** 가능하여, 게임의 모든 부분에서 쉽게 사용할 수 있습니다.
    * 독립적 기능 모듈화: 각 SubSystem은 독립적으로 기능을 모듈화하여, 특정 기능을 관리하고 사용할 수 있게 합니다.
자동 초기화 및 종료: 엔진이 실행될 때 자동으로 초기화되며, 종료될 때 자동으로 정리됩니다.
* 엔진 서브시스템 (Engine SubSystem)
    * 엔진 전체에 걸쳐 사용되는 서브시스템으로, 엔진 레벨에서의 전역적인 기능을 제공합니다.
    * 예: 네트워크 관리, 파일 시스템 관리 등.
* 게임 인스턴스 서브시스템 (Game Instance SubSystem)
    * 특정 게임 인스턴스에 속하는 서브시스템으로, 주로 게임 실행 동안 지속적으로 필요한 기능을 제공합니다.
    * 예: 세션 관리, 저장 및 로드 시스템 등.
* 로컬 플레이어 서브시스템 (Local Player SubSystem)
    * 특정 로컬 플레이어와 관련된 서브시스템으로, 플레이어별로 필요한 기능을 제공합니다.
    * 예: 입력 관리, 플레이어 설정 등.
* 월드 서브시스템 (World SubSystem)
    * 특정 월드에 관련된 서브시스템으로, 월드(레벨)별로 필요한 기능을 제공합니다.
    * 예: 월드 상태 관리, AI 시스템 등.

---

* DragAndDrop 처리
    * Operation을 객체로 만듦.

```cpp
UCLASS()
class R1_API UR1DragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UR1DragDropOperation(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	FIntPoint FromItemSlotPos = FIntPoint::ZeroValue;

	UPROPERTY()
	TObjectPtr<UR1ItemInstance> ItemInstance;

	FVector2D DeltaWidgetPos = FVector2D::ZeroVector;
};
```

```cpp
void UR1InventoryEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, OUT UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	 
	UR1ItemDragWidget* DragWidget = CreateWidget<UR1ItemDragWidget>(GetOwningPlayer(), DragWidgetClass);
	FVector2D EntityWidgetSize = FVector2D(1 * 50, 1 * 50);
	DragWidget->Init(EntityWidgetSize, nullptr, ItemCount);

	UR1DragDropOperation* DragDrop = NewObject<UR1DragDropOperation>();
	DragDrop->DefaultDragVisual = DragWidget;
	DragDrop->Pivot = EDragPivot::MouseDown;
	DragDrop->FromItemSlotPos = CachedFromSlotPos;
	DragDrop->ItemInstance = ItemInstance;
	DragDrop->DeltaWidgetPos = CachedDeltaWidgetPos;

	OutOperation = DragDrop;
}
```

```cpp
UCLASS()
class R1_API UR1ItemDragWidget : public UR1UserWidget
{
	GENERATED_BODY()

public:
	UR1ItemDragWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void Init(const FVector2D& InWidgetSize, UTexture2D* InItemIcon, int32 InItemCount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Count;
};
```