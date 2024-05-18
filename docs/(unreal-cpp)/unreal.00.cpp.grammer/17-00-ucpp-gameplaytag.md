---
layout: default
title: "17. GamePlayTag"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* Project Settings -> Project -> GameplayTags

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-17-1.png"/>
</p>

* Manage Gameplay Tags... 클릭

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-17-2.png"/>
</p>

* Input.Action.Move / Turn을 이렇게 만들수도 있고 C++을 통해만들수도 있다.
    * C++을 통해 만드는것을 권장!

```cpp
namespace R1GameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Turn);
}
```

```cpp
namespace R1GameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Move, "Input.Action.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Turn, "Input.Action.Turn");

}
```

* 참고로 이 코드만 넣어도 Project Settings -> Project -> GameplayTags 에서 자동으로 Tags가 생성된다.
* 이걸이제 어떻게 쓰지?

```cpp
void AR1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const UR1InputData* InputData = UR1AssetManager::GetAssetByName<UR1InputData>("InputData"))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

        // 뭔지는 모르겠지만 일단 이렇게 씀.
		auto Action1 = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Move);
		EnhancedInputComponent->BindAction(Action1, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);

		auto Action2 = InputData->FindInputActionByTag(R1GameplayTags::Input_Action_Turn);
		EnhancedInputComponent->BindAction(Action2, ETriggerEvent::Triggered, this, &ThisClass::Input_Turn);
	}
}
```

```cpp
const UInputAction* UR1InputData::FindInputActionByTag(const FGameplayTag& InputTag) const
{
    // UR1InputData에서 관리되는 InputActions을 Tag를 통해 리턴
	for (const FR1InputAction& Action : InputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	UE_LOG(LogR1, Error, TEXT("Can't find InputAction for InputTag [%s]"), *InputTag.ToString());

	return nullptr;
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-17-3.png"/>
</p>