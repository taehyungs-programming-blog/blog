---
layout: default
title: "02. 무기 + 피격판정"
parent: "(Unreal C++ RPG Action만 구현)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 무기를 붙일 Socket을 만들어 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-2-1.png"/>
</p>

* 위 화면에 이미 무기가 붙어있는건, 우클릭 후 Add Preview Asset으로 추가해 뒀기 때문이다.

---

## 무기를 상속받을 Weapon클래스를 만들어보자

* Actor클래스 C++기반으로 생성

```cpp
UCLASS()
class PROJECTRPG_API APlayerWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerWeapon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* WeaponMesh;

    // ...
```

```cpp
void APlayerWeapon::BeginPlay(){
    // ...

	WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	FName WeaponSocket = FName(TEXT("WeaponPivotSocket"));

	WeaponCollision->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
}
```

* 만드는 김에 Equip, UnEquip까지 

```cpp
void APlayerWeapon::EquipWeapon(const APlayerWeapon* Weapon)
{
	/** 만일 플레이어를 찾았다면? */
	if (PlayerPawn != nullptr)
	{
		USkeletalMeshComponent* PlayerMesh = PlayerPawn->GetMesh();
		
		FName WeaponSocket = PlayerPawn->GetWeaponSocketName();

		WeaponMesh->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}
}

void APlayerWeapon::UnEquipWeapon()
{
	WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}
```

---

## Blueprint로 Weapon List 받기

```cpp
// AProjectRPGCharacter

private:
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (AllowPrivateAccess = true))
	FName RightWeaponSocket;

	UPROPERTY()
	TArray<class APlayerWeapon*> Weapons;

	UPROPERTY()
	class APlayerWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TArray<TSubclassOf<class APlayerWeapon>> BP_Weapons;
```

```cpp
void AProjectRPGCharacter::SpawnPlayerWeapon()
{
	for (int32 i = 0; i < BP_Weapons.Num(); i++)
	{
		if (BP_Weapons[i])
		{
			FActorSpawnParameters SpawnParams;

            // SubObject의 경우 SpawnActor로 생성함을 유의
			APlayerWeapon* Weapon = GetWorld()->SpawnActor<APlayerWeapon>(BP_Weapons[i], SpawnParams);
			
			if (Weapon != nullptr)
			{
				Weapons.Add(Weapon);
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("%d 종의 무기가 생성되었습니다. !!"), Weapons.Num());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::MakeRandomColor(), 
			FString::Printf(TEXT("%d 종의 무기가 생성되었습니다. !!"), Weapons.Num()));
	}

	if (Weapons.Num() > 0)
	{
		EquipWeapon(Weapons[(int32)EWeaponType::Blade_BlackKnight]);
	}
}
```

---

## Weapon Blueprint를 만들어 보자

* 동일하게 만들고 Mesh만 바꿔끼면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-2-2.png"/>
</p>


