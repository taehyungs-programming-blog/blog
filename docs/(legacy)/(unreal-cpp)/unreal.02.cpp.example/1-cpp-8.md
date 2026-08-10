---
layout: default
title: "08. 아이템 추가"
parent: "(Example - RPG Action)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 우선 item을 enum으로 정의하자

```cpp
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(DisplayName = "None"),			
	Weapon UMETA(DisplayName = "Weapon"),		
	UpperBody UMETA(DisplayName = "UpperBody"), 
	LowerBody UMETA(DisplayName = "LowerBody"), 
	Hair UMETA(DisplayName = "Hair"),
};
```

```cpp
UCLASS()
class PROJECTRPG_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Item", meta = (AllowPrivateAccess = true))
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (AllowPrivateAccess = true))
	EWeaponItemType WeaponItemType;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (AllowPrivateAccess = true))
	EUpperBodyItemType UpperBodyItem;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (AllowPrivateAccess = true))
	ELowerBodyItemType LowerBodyItemType;

	UPROPERTY(EditAnywhere, Category = "Item", meta = (AllowPrivateAccess = true))
	EHairItemType HairItemType;
	
	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item", meta = (AllowPrivateAccess = true))
	class UBoxComponent* BoxComponent;


public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
```