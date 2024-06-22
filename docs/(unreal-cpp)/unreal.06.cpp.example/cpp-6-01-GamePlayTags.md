---
layout: default
title: "01. LyraGameplayTags"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/LyraClone/tree/1)

---

## Gameplay Tag란 뭘까?

* GameplayTag
    * 일종의 글로벌 정의(Global Define)로 볼 수 있긴 하지만, 
    * 좀 더 정확히는 유연하고 확장 가능한 **메타데이터 시스템**이라고 보는 것이 적합하다. 
    * 글로벌 정의는 주로 컴파일 시간에 결정되는 상수값을 의미하며, 프로그램 전반에 걸쳐 변경되지 않는 값을 지정하는 데 사용된다. 
    * 반면, GameplayTag는 런타임에도 추가, 수정, 조회가 가능하며 게임의 다양한 엔티티와 상호작용을 동적으로 태깅하고 분류하는 데 사용된다.

* **대략 게임 전반적으로 사용되는 변수? 혹은 Define이라 생각하자.**

---

## 어떻게 쓸까?

### Gameplay Tag 등록

* 참고로 GameplayTags는 Moudle을 추가해야 사용이 가능하다.

```cpp
namespace LyraGameplayTags
{
	LYRAGAME_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that Lyra will use
    // 대략 이런식으로 선언한다.
	LYRAGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);

    // ...
```

* 이게 추가되면 어디에 등록이 될까?
    * Project Settings -> Project -> GameplayTags

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-1-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-1-2.png"/>
</p>

* 아래 예시는 딱히 GitHub Repo의 소스코드는 아님.

```cpp
#include "GameplayTagContainer.h"

// GameplayTag 가져오기
FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Character.Ability.Damage"));

// 캐릭터에 대한 특정 능력이나 상태를 체크하는 함수
void CheckCharacterAbilities(const FGameplayTagContainer& CharacterTags)
{
    if (CharacterTags.HasTag(DamageTag))
    {
        // 태그가 있을 경우, 캐릭터는 'Damage' 능력을 가지고 있음을 의미
        UE_LOG(LogTemp, Log, TEXT("This character can deal damage."));
    }
    else
    {
        // 태그가 없을 경우
        UE_LOG(LogTemp, Log, TEXT("This character cannot deal damage."));
    }
}

// 메인 함수나 다른 적절한 위치에서 GameplayTagContainer 사용
int main()
{
    // 캐릭터 태그 컨테이너 생성
    FGameplayTagContainer CharacterTags;

    // 태그 추가
    CharacterTags.AddTag(DamageTag);

    // 능력 체크
    CheckCharacterAbilities(CharacterTags);

    return 0;
}
```

---

### Github Repo예시에선 어떻게 썼나?

* 여기서 부터 알아야 할게 많음. (그래서 이 예시를 아래에 둠.)
    * Q. A. 로 설명을 진행하겠다.

```cpp
// Q. ULyraCheatManager는 어디쓰는 애인가?
void ULyraCheatManager::AddTagToSelf(FString TagName)
{
	FGameplayTag Tag = LyraGameplayTags::FindTagByString(TagName, true);

	if (Tag.IsValid())
	{
        // Q. ULyraAbilitySystemComponent는 어디쓰는 애인가?
        // Q. GetPlayerAbilitySystemComponent()는 뭘 가져오나?
		if (ULyraAbilitySystemComponent* LyraASC = GetPlayerAbilitySystemComponent())
		{
			LyraASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogLyraCheat, Display, TEXT("AddTagToSelf: Could not find any tag matching [%s]."), *TagName);
	}
}
```

---

### ULyraCheatManager

#### UMyCheatManager 란?

* ULyraCheatManager는 UCheatManager의 자손이다

```cpp
class ULyraCheatManager : public UCheatManager
```

* Unreal Engine의 UCheatManager는 게임 내 치트 코드를 처리하고 관리하는 데 사용되는 클래스입니다. 
* 이 클래스는 주로 게임 **개발 중 테스트 및 디버깅을 위해 사용**됩니다. 
* UCheatManager를 통해 개발자는 특정 명령을 사용하여 게임 내에서 **다양한 치트를 활성화할 수 있습니다.**
* 이러한 치트 명령은 일반적으로 콘솔을 통해 입력되며, 게임의 다양한 측면을 테스트하는 데 유용합니다.

```cpp
UCLASS()
class MYGAME_API UMyCheatManager : public UCheatManager
{
   GENERATED_BODY()

public:
   UFUNCTION(exec)
   void MyCustomCheat();
};

// UMyCheatManager를 치면 아래가 실행
void UMyCheatManager::MyCustomCheat()
{
   UE_LOG(LogTemp, Warning, TEXT("My custom cheat activated!"));
}
```

* 다시 ULyraCheatManager로 돌아가보자
    * `AddTagToSelf`를 치면되겠구나?

```cpp
class ULyraCheatManager : public UCheatManager
{
	GENERATED_BODY()

    // ...

    // Adds the dynamic tag to the owning player's ability system component.
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	virtual void AddTagToSelf(FString TagName);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-1-3.png"/>
</p>

---

### ULyraAbilitySystemComponent

* 역시 같은 방법으로 분석

```cpp
class LYRAGAME_API ULyraAbilitySystemComponent : public UAbilitySystemComponent
```

#### UAbilitySystemComponent 란?

* Unreal Engine의 UAbilitySystemComponent는 복잡한 게임플레이 능력과 효과 시스템을 관리하기 위한 매우 강력한 컴포넌트입니다. 
* 특히 액션 RPG와 같은 복잡한 게임에서 다양한 능력(스킬)과 효과(버프, 디버프)를 효율적으로 구현하고 관리할 수 있도록 돕습니다.
    * 사실 AbilitySystem은 방대하여 여기서는 간단하게 설명한다.

* 주요구성요소
    * **Gameplay Ability:**
        * UGameplayAbility 클래스는 특정 **능력을 정의**합니다. 각 능력은 실행 로직, 조건, 비용 등을 포함합니다.
    * **Gameplay Effect:**
        * UGameplayEffect 클래스는 능력 사용 시 적용되는 **효과를 정의**합니다. 예를 들어, 일정 시간 동안 캐릭터의 공격력을 증가시키는 효과를 정의할 수 있습니다.
    * **Gameplay Tag:**
        * 태그 시스템은 특정 상태나 **조건을 정의**하는데 사용됩니다. 예를 들어, "Stunned", "Poisoned"와 같은 태그를 통해 캐릭터의 상태를 관리할 수 있습니다.
    * **Attribute Set:**
        * UAttributeSet 클래스는 **캐릭터의 속성을 정의**합니다. 각 속성은 UProperty로 정의되며, 능력과 효과에 따라 변경될 수 있습니다.

* 능력 정의 예시

```cpp
// MyGameplayAbility.h
UCLASS()
class MYGAME_API UMyGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UMyGameplayAbility();

    // 능력을 활성화할 때 호출되는 함수
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                 const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                 const FGameplayEventData* TriggerEventData) override;
};
```

```cpp
// MyGameplayAbility.cpp
UMyGameplayAbility::UMyGameplayAbility()
{
    // 능력의 초기화 코드, 예를 들어, 비용 설정 등
}

void UMyGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
    if (ActorInfo->AvatarActor.IsValid())
    {
        ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
        if (Character)
        {
            // 캐릭터의 속도를 증가시킴
            Character->GetCharacterMovement()->MaxWalkSpeed *= 2.0f;
        }

        // 능력을 완료로 표시
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}
```

* 능력 활성화 예시

```cpp
// MyCharacter.cpp

// 능력을 활성화하는 함수
void AMyCharacter::ActivateMyAbility()
{
    if (AbilitySystemComponent)
    {
        // MyGameplayAbility 클래스를 활성화
        AbilitySystemComponent->TryActivateAbilityByClass(UMyGameplayAbility::StaticClass());
    }
}
```

* 다시 ULyraAbilitySystemComponent로 돌아가보자

```cpp
UCLASS()
class LYRAGAME_API ULyraAbilitySystemComponent : public UAbilitySystemComponent
{
    // ...

    // Uses a gameplay effect to add the specified dynamic granted tag.
	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);
```

```cpp
void ULyraAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = ULyraAssetManager::GetSubclass(ULyraGameData::Get().DynamicTagGameplayEffect);
	if (!DynamicTagGE)
	{
		UE_LOG(LogLyraAbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to find DynamicTagGameplayEffect [%s]."), *ULyraGameData::Get().DynamicTagGameplayEffect.GetAssetName());
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		UE_LOG(LogLyraAbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to make outgoing spec for [%s]."), *GetNameSafe(DynamicTagGE));
		return;
	}

	Spec->DynamicGrantedTags.AddTag(Tag);

	ApplyGameplayEffectSpecToSelf(*Spec);
}
```

---

### GetPlayerAbilitySystemComponent() 는 뭘 가져오나?

```cpp
ULyraAbilitySystemComponent* ULyraCheatManager::GetPlayerAbilitySystemComponent() const
{
	if (ALyraPlayerController* LyraPC = Cast<ALyraPlayerController>(GetOuterAPlayerController()))
	{
		return LyraPC->GetLyraAbilitySystemComponent();
	}
	return nullptr;
}
```

#### ACommonPlayerController 란?

* ACommonPlayerController는 Unreal Engine에서 플레이어 컨트롤러의 기능을 확장하기 위해 사용하는 클래스입니다. 
* 이 클래스는 보통 APlayerController의 기능을 기반으로 하고 있으며, 특히 멀티플레이어 게임이나 특정 게임플레이 메커니즘을 구현할 때 유용합니다. 
* ACommonPlayerController 클래스는 일반적으로 사용자 입력을 처리하고, 카메라 제어를 관리하며, 플레이어의 상호작용을 처리하는 데 사용됩니다.

```cpp
// MyPlayerController.h

UCLASS()
class MYGAME_API AMyPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AMyPlayerController();

protected:
    virtual void SetupInputComponent() override;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartJump();
    void StopJump();
};
```

```cpp
// MyPlayerController.cpp

AMyPlayerController::AMyPlayerController()
{
    // 생성자 코드
}

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // 입력 매핑
    InputComponent->BindAxis("MoveForward", this, &AMyPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AMyPlayerController::MoveRight);
    InputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerController::StartJump);
    InputComponent->BindAction("Jump", IE_Released, this, &AMyPlayerController::StopJump);
}

void AMyPlayerController::MoveForward(float Value)
{
    if (APawn* MyPawn = GetPawn())
    {
        MyPawn->AddMovementInput(FVector::ForwardVector, Value);
    }
}

void AMyPlayerController::MoveRight(float Value)
{
    if (APawn* MyPawn = GetPawn())
    {
        MyPawn->AddMovementInput(FVector::RightVector, Value);
    }
}

void AMyPlayerController::StartJump()
{
    if (ACharacter* MyCharacter = Cast<ACharacter>(GetPawn()))
    {
        MyCharacter->Jump();
    }
}

void AMyPlayerController::StopJump()
{
    if (ACharacter* MyCharacter = Cast<ACharacter>(GetPawn()))
    {
        MyCharacter->StopJumping();
    }
}
```

---

## (Tips) Log를 등록해 로그 관리를 좀 편하게 해보자

```cpp
// LyraLogChannels.h

#pragma once

#include "Logging/LogMacros.h"

class UObject;

// 우선은 LogChannel을 추가
LYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyra, Log, All);
LYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraExperience, Log, All);
LYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraAbilitySystem, Log, All);
LYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyraTeams, Log, All);

LYRAGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
```