---
layout: default
title: "14. (Lyra) UI"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UI 디버깅을 위해서 찾가가는 방법

* esc를 누르면 나타나는 화면에서 부터 시작하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-14-1.png"/>
</p>

* 그런데 이걸 어디서 부터 찾지??
* 보통이런건 Key-map을 위해서 GameTag에 정의되어있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-14-2.png"/>
</p>

* `UI.Action.Escape`를 찾아보자

```cpp
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");

ULyraHUDLayout::ULyraHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULyraHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
}

void ULyraHUDLayout::HandleEscapeAction()
{
	if (ensure(!EscapeMenuClass.IsNull()))
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), TAG_UI_LAYER_MENU, EscapeMenuClass);
	}
}
```

---

## 총알을 관리하는 위젯을 보고싶다면?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-14-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-14-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-14-5.png"/>
</p>

* 이건 생긴게 명확해 찾기는 쉽구만

```cpp
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class ULyraQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

    // ...
```

