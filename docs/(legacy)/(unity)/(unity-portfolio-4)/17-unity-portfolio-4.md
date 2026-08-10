---
layout: default
title: "17. Unreal GAS(Game Ability System)"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unity.IncrementalGame.Example/tree/6)

---

## 현재의 Skill System이 어떻게 동작하나 보자.

```csharp
// DataManager에서 JSON으로 기록된 SkillData를 읽어온다

public void Init()
{
    // ...
    SkillDic = LoadJson<Data.SkillDataLoader, int, Data.SkillData>("SkillData").MakeDict();
```

```csharp
// SkillBase에서는 SetInfo시 기록된 SkillData를 사용

public virtual void SetInfo(Creature owner, int skillTemplateID)
{
    Owner = owner;
    SkillData = Managers.Data.SkillDic[skillTemplateID];

    // Register AnimEvent
    if (Owner.SkeletonAnim != null && Owner.SkeletonAnim.AnimationState != null)
    {
        Owner.SkeletonAnim.AnimationState.Event -= OnOwnerAnimEventHandler;
        Owner.SkeletonAnim.AnimationState.Event += OnOwnerAnimEventHandler;
    }
}
```

```csharp
// 이러면 구조상 SkillBase를 상속하는 Skill을 만들어야하는 상황이다.
public class NormalAttack : SkillBase
```

* 이게 효율적일까?
* 어려운 점이... Item에서 Skill에 영향을 주는케이스
* 버프, 디버프가 있는케이스 등 SkillSystem에 영향을 줄수있는 항목이 다양하다
* 다른 엔진은 어떻게 만들었을까?

---

## Unreal Engine의 GAS(Game Ability System)을 보자

* Blueprint에서 **GamePlayEffect**, **GameplayAbility**를 만들어보자.
* GamePlayEffect에 Duration(바로발동 혹은 서서히 발동), Modifiers(어떤영향을 줄지) 등을 수정해 Object에 붙일수 있음.

```cpp
// MyHealthRegenEffect.h
#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "MyHealthRegenEffect.generated.h"

UCLASS()
class MYGAME_API UMyHealthRegenEffect : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UMyHealthRegenEffect();
};
```

```cpp
// MyHealthRegenEffect.cpp
#include "MyHealthRegenEffect.h"

UMyHealthRegenEffect::UMyHealthRegenEffect()
{
    // 설정 예시: 지속 시간 5초, 체력 회복량 10
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FGameplayEffectModifierMagnitude(5.0f);
    InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Effect.HealthRegen")));

    // 체력 회복을 위한 Modifier 정의
    FGameplayModifierInfo ModifierInfo;
    ModifierInfo.ModifierMagnitude = FScalableFloat(10.0f);
    ModifierInfo.ModifierOp = EGameplayModOp::Additive;
    ModifierInfo.Attribute = UAttributeSetHealth::GetHealthAttribute();

    Modifiers.Add(ModifierInfo);
}
```

```cpp
// AMyCharacte도 IAbilitySystemInterface을 쓴다고 알린다
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AMyCharacter.generated.h"

UCLASS()
class MYGAME_API AMyCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AMyCharacter();

    // IAbilitySystemInterface 구현
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
    // 캐릭터의 AbilitySystemComponent
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UAbilitySystemComponent* AbilitySystemComponent;

    // 기타 멤버 변수 및 함수 선언...
};

```

```cpp
#include "AMyCharacter.h"

AMyCharacter::AMyCharacter()
{
    // AbilitySystemComponent 초기화
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

UAbilitySystemComponent* AMyCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

```

```cpp
// 캐릭터 클래스나 어빌리티 클래스 내부에서

void AMyCharacter::ApplyHealthRegenEffect()
{
    if (AbilitySystemComponent)
    {
        FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(UMyHealthRegenEffect::StaticClass(), 1.0f, AbilitySystemComponent->MakeEffectContext());

        if (EffectSpecHandle.IsValid())
        {
            FActiveGameplayEffectHandle ActiveEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
        }
    }
}
```