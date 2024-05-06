---
layout: default
title: "02. ULyraAssetManager"
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

## ULyraAssetManager란?

* Asset(Resource)를 로드 / 메모리관리 등을 관장하게 된다.
* 그럼 Lyra에서 ULyraAssetManager를 어떻게 사용하는지 먼저 확인하자.
    * Project Settings -> Game -> Asset Manager -> Primary Asset Types to Scan에 Asset을 세팅한 것을 볼 수 있다.
    * 아직 뭔지는 모르겠지만 Project Setting에서 세팅을해 넘겨준다고 이해하자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-2-1.png"/>
</p>

* 세부적으로 보자면 넘겨준 데이터 중 DefaultGameData가 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-2-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-2-3.png"/>
</p>

* 역시 아직은 뭔진모르겠으나 Effect를 넘겨준다.

---

## UE에서 Asset Manager 설정하기

* 아래처럼 project settings에서 asset manager클래스를 지정해 줘야한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-2-4.png"/>
</p>

```cpp
UCLASS(Config = Game)
class ULyraAssetManager : public UAssetManager
{
	GENERATED_BODY()

    // ...
```

---

## 어떻게 사용할까?

* 아래처럼 세팅하고

```cpp
// LyraGameData.h

#pragma once

#include "Engine/DataAsset.h"

#include "LyraGameData.generated.h"

class UGameplayEffect;
class UObject;

/**
 * ULyraGameData
 *
 *	Non-mutable data asset that contains global game data.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Lyra Game Data", ShortTooltip = "Data asset containing global game data."))
class ULyraGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	ULyraGameData();

	// Returns the loaded game data.
	static const ULyraGameData& Get();

public:

	// Gameplay effect used to apply damage.  Uses SetByCaller for the damage magnitude.
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

	// Gameplay effect used to apply healing.  Uses SetByCaller for the healing magnitude.
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;

	// Gameplay effect used to add and remove dynamic tags.
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects")
	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
};
```

* 아래 처럼 사용한다.

```cpp
void ULyraAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = 
    ULyraAssetManager::GetSubclass(ULyraGameData::Get().DynamicTagGameplayEffect);

    // ...
```

---

## (Tips) UCLASS(config=Game) 는 뭘까?

```cpp
UCLASS(config=Game)
```

* .ini에 세팅을 박아둬서 로딩시 가져온다를 의미.

