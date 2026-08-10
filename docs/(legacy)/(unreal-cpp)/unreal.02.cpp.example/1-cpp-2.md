---
layout: default
title: "02. 무기 + 피격판정"
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

---

## 공격 애니메이션 처리

* 애니메이션 우 클릭 후 -> 애니메이션 에셋 리타깃

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-2-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-2-4.png"/>
</p>

* 생성된 애니메이션 우 클릭 후 -> Anim 몽타주 생성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_1/1-cpp-2-5.png"/>
</p>

* PlayerWeapon 클래스에서 공격 애니메이션을 처리해 보자

```cpp
UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
TArray<class UAnimMontage*> AttackAnimMontages;
```

```cpp
void APlayerWeapon::StartAttack()
{
	if (bIsAttack == false)
	{
		if (AttackIndex > 3)
		{
			AttackIndex = 0;
		}

		float AnimDuration = PlayerPawn->PlayAnimMontage(AttackAnimMontages[AttackIndex]);

		bIsAttack = true;

		AttackIndex++;

		FTimerHandle TimerHandle;

		GetWorldTimerManager().SetTimer(TimerHandle, this, &APlayerWeapon::EndAttack, AnimDuration);
	}
}
```

---

## Weapon 충돌(공격) 처리

* 우선 Player에 Health를 두자

```cpp
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health", meta = (AllowPrivateAccess = true))
	float Health;

    // 넣는김에 Hit, Die Anim도 넣어두자
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health", meta = (AllowPrivateAccess = true))
	UAnimMontage* HitAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health", meta = (AllowPrivateAccess = true))
	UAnimMontage* DeathAnimMontage;
```

```cpp
// TakeDamage는 Unreal에서 지원해 주는 함수이다.
float AProjectRPGCharacter::TakeDamage (
    float Damage, FDamageEvent const& DamageEvent, 
    AController* EventInstigator, AActor* DamageCauser)
{
	
	if (Health <= 0.0f)
	{
		return 0.0f;
	}
	
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	if (ActualDamage > 0)
	{
		Health -= ActualDamage;
	}
	
	if (Health <= 0.0f)
	{
		Die();
	} 
	else
	{
		Hit();
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Player's Health : %f"), Health);

	return ActualDamage;
}
```

* 자! 이제 충돌을 처리할 Collision을 넣자
* Weapon 클래스에 아래를 추가

```cpp
private:
	UPROPERTY(VisibleAnywhere, Category = "Collision", meta = (AllowPrivateAccess = true))
	class USphereComponent* WeaponCollision;

public:
    // Weapon에 Overlap(충돌)됐다면 여기가 호출됨.
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:

public:
	void SetWeaponCollisionEnabled();

	void SetWeaponCollisionDisabled();
```

```cpp
WeaponCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponCollision"));

WeaponCollision->InitSphereRadius(10.0f);


WeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

// WeaponPivotSocket이란 Socket을 각 Weapon에 추가해야함을 잊지말자
FName WeaponSocket = FName(TEXT("WeaponPivotSocket"));

WeaponCollision->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
```