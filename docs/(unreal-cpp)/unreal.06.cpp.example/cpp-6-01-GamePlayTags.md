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

### 시작 전, Log를 등록해 로그 관리를 좀 편하게 해보자

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

---

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
