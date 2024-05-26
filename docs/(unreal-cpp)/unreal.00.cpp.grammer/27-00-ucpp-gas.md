---
layout: default
title: "27. GAS(Game Ability System)"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/5)

---

## Ability System Component 생성 

* Ability System Component의 관리는
    * Player - PlayerState Class를 하나 만들어 보자
    * Monster 외 기타 - Character Class

```cpp
UCLASS()
class R1_API UR1AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& StartupAbilities);
	
	void ActivateAbility(FGameplayTag AbilityTag);

	TArray<FGameplayAbilitySpecHandle> SpecHandles;

};
```

```cpp
UCLASS()
class R1_API AR1PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AR1PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	UR1AbilitySystemComponent* GetR1AbilitySystemComponent() const;
	UR1PlayerSet* GetR1PlayerSet() const;

protected:
    // 여기서 Ability System을 관리하게 된다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UR1AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UR1PlayerSet> PlayerSet;
};
```

```cpp
UCLASS()
class R1_API AR1Character : public ACharacter, public IR1HighlightInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

    // ...

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UR1AbilitySystemComponent> AbilitySystemComponent;
```

* 잘됐나 확인 `$ showdebug abilitysystem`
* 참조하면 좋을 사이트 [GASDocumentation 🌟](https://github.com/tranek/GASDocumentation)

---

## Gameplay Attribute

* Stat을 관리할 클래스를 정의해 보자.

```cpp
UCLASS()
class R1_API UR1AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UR1AttributeSet();
	
public:
	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass, Mana);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxMana);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, BaseDefense);


private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxMana;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData BaseDefense;
};
```

```cpp
void AR1Character::OnDamaged(int32 Damage, TObjectPtr<AR1Character> Attacker)
{
	float Hp = AttributeSet->GetHealth();
	float MaxHp = AttributeSet->GetMaxHealth();

	Hp = FMath::Clamp(Hp - Damage, 0, MaxHp);
	AttributeSet->SetHealth(Hp);

	if (Hp == 0)
	{
		OnDead(Attacker);
	}

	RefreshHpBarRatio();
}
```

---

## Gameplay Effect

* 하나 만들며 보면 편하다
    * 주의할 점은 Effect가 그래픽의 Effect가 아니라 Buff의 개념으로 생각해야함. 주의.
* Blueprint 생성 -> GameplayEffect

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-27-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-27-2.png"/>
</p>

* 이걸 어떻게 반영할까?

```cpp
UCLASS()
class R1_API AR1Player : public AR1Character
{
	GENERATED_BODY()

    // ...

protected:
	// TEMP
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UGameplayEffect> TestEffect;
```

```cpp
void AR1Player::BeginPlay()
{
	Super::BeginPlay();

	// TEMP
	if (TestEffect && AbilitySystemComponent)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		// Handle
		FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(TestEffect, 1, EffectContext);

		// Apply
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}
```

---

## Example Gameplay Attribute Attack 만들어 보기

```cpp
UCLASS()
class R1_API UR1GameplayAbility_Attack : public UR1GameplayAbility
{
	GENERATED_BODY()
		
public:
	UR1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage;
};
```

```cpp
UCLASS()
class R1_API AR1Character : public ACharacter, public IR1HighlightInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

    // ...

public:
	void AddCharacterAbilities();

	UPROPERTY(EditAnywhere, Category=Abilities)
	TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;
};
```

```cpp
void AR1Character::AddCharacterAbilities()
{
	UR1AbilitySystemComponent* ASC = Cast<UR1AbilitySystemComponent>(AbilitySystemComponent);
	if (ASC == nullptr)
	{
		return;
	}

    // UR1AbilitySystemComponent에 StartupAbilities을 등록.
	ASC->AddCharacterAbilities(StartupAbilities);
}
```

```cpp

void AR1PlayerController::ChaseTargetAndAttack()
{
	// ..

    // 실행명령
    R1Player->ActivateAbility(R1GameplayTags::Ability_Attack);
```

```cpp
void AR1Player::ActivateAbility(FGameplayTag AbilityTag)
{
	AbilitySystemComponent->ActivateAbility(AbilityTag);
}
```

```cpp
void UR1AbilitySystemComponent::ActivateAbility(FGameplayTag AbilityTag)
{
	for (FGameplayAbilitySpecHandle& SpecHandle : SpecHandles)
	{
		// TODO
		TryActivateAbility(SpecHandle);
	}
}
```