---
layout: default
title: "27. GAS(Game Ability System)"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/5)

---

## 선-정리

* `UGameplayAbility`를 통하여 BP에서 어떤 Effect가 Activate되게할지 지정할수 있다.
* `UGameplayAbility`의 Activate는 `AR1Player`와 같은 Player에서 관장할수 있다.
* `AR1Player`와 같은 Player도 `UGameplayAbility`를 직접적으로 알수는 없지만 간접적으로 `UR1AbilitySystemComponent`와 같은 클래스를 통해 알게 된다.
* `AR1PlayerState`와 같은 State에서는 Activate할 Ability와 Set(Data정보)를 관리하게 된다.

---

## 실전으로 익혀보자!

* `UGameplayAbility`를 베이스로 클래스를 하나 만든다
	* `UR1GameplayAbility`는 abstract이다.

```cpp
UCLASS()
class R1_API UR1GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
};
```

```cpp
// 실제 구현체는 UR1GameplayAbility_Attack이다.
UCLASS()
class R1_API UR1GameplayAbility_Attack : public UR1GameplayAbility
{
	GENERATED_BODY()
		
public:
	UR1GameplayAbility_Attack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// 이런 가상함수를 채워 넣어야 한다.
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> AttackMontage;
};
```

* `UR1GameplayAbility_Attack`를 베이스로 Blue-Print를 만들어 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-27-3.png"/>
</p>

* 뭔지는 모르겠지만 Montage를 넣어놨다.
* 느낌적 느낌에 어딘가에서 `UR1GameplayAbility_Attack`를 Activate할 시 Montage가 실행됨을 안다면 100%이해한 것이다.
* 자! 그럼 어디서 `UR1GameplayAbility_Attack`를 Activate하게 될 것인가

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-27-4.png"/>
</p>

* Player에서 로드중이군!

```cpp
UCLASS()
class R1_API AR1Player : public AR1Character
{
	GENERATED_BODY()

public:
	// ActivateAbility를 통해 Activate중이다.
	void ActivateAbility(FGameplayTag AbilityTag);

	// 그런데 신기한건
		// UR1GameplayAbility_Attack를 바로 Ativate하는게 아니라
		// UR1AbilitySystemComponent를 통해 Activate한다는 점이다.
	protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UR1AbilitySystemComponent> AbilitySystemComponent;
```

## AbilitySystemComponent

```cpp
UCLASS()
class R1_API UR1AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// 캐릭터별 Ability를 관리
	void AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& StartupAbilities);
	
	// Activate
	void ActivateAbility(FGameplayTag AbilityTag);

	TArray<FGameplayAbilitySpecHandle> SpecHandles;

};
```

```cpp
// 가령 이런식이다.
void AR1Character::AddCharacterAbilities()
{
	UR1AbilitySystemComponent* ASC = Cast<UR1AbilitySystemComponent>(AbilitySystemComponent);
	if (ASC == nullptr)
	{
		return;
	}

	// StartupAbilities값은 BP로 Eidtor에서 넣어준다
	ASC->AddCharacterAbilities(StartupAbilities);
}
```

* 그럼 다시 AR1Player::ActivateAbility 로 돌아가서 ...
* AR1Player에서 UR1AbilitySystemComponent를 직접적으로 알고 있는 식일까?
	* 그건 Nope!

```cpp
void AR1Player::InitAbilitySystem()
{
	Super::InitAbilitySystem();

	// AR1Player는 UR1AbilitySystemComponent를 직접적으로 알지는 못하고
	// State를 통해서 알게 된다.
	if (AR1PlayerState* PS = GetPlayerState<AR1PlayerState>())
	{
		AbilitySystemComponent = Cast<UR1AbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		AttributeSet = PS->GetR1PlayerSet();
	}
}
```

* 그럼 PlayerState는 뭘까?

## PlayerState

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
	// AbilitySystemComponent과
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UR1AbilitySystemComponent> AbilitySystemComponent;

	// PlayerSet을 관리하게 된다.
		// class R1_API UR1PlayerSet : public UR1AttributeSet
	UPROPERTY()
	TObjectPtr<UR1PlayerSet> PlayerSet;
};
```

* 참고로 `$ showdebug abilitysystem`를 통해 GameAbilty System이 잘동작하나 확인도 가능하다.
	* 참조하면 좋을 사이트 [GASDocumentation 🌟](https://github.com/tranek/GASDocumentation)

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
