---
layout: default
title: "12. (Lyra) 데미지 판정"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* 우선 어디서 부터 봐야할지 찾는게 문제
* 데미지니 아무래도 무기 쪽?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-12-1.png"/>
</p>

* 역시 Weapon에 뭔가 붙어있군
* 돌고 돌아 GA_Weapon_Fire로 간다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-12-2.png"/>
</p>

```cpp
// 결국 여기를 확인하면 됨.
void ULyraGameplayAbility_RangedWeapon::StartRangedWeaponTargeting()
{
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

    // ...
```

* 데미지 판정은 여기로

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-12-3.png"/>
</p>

```cpp
// Called when target data is ready
UFUNCTION(BlueprintImplementableEvent)
void OnRangedWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);
```


