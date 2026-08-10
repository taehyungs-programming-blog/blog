---
layout: default
title: "23. highlight"
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

* [Get Code 🌟](https://github.com/Arthur880708/UnrealEngineGrammer/tree/2)

---

* Unreal Interface를 하나 만들자.

```cpp
UINTERFACE(MinimalAPI)
class UR1HighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class R1_API IR1HighlightInterface
{
	GENERATED_BODY()

public:
	virtual void Highlight() = 0;
	virtual void UnHighlight() = 0;
};
```

```cpp
class R1_API AR1Character : public ACharacter, public IR1HighlightInterface
{
    // ...

	virtual void Highlight() override;
	virtual void UnHighlight() override;
}
```

```cpp
void AR1Character::Highlight()
{
	bHighlighted = true;
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(250);
}

void AR1Character::UnHighlight()
{
	bHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
}
```

```cpp
// 컨트롤러에서 하이라이트 처리
void AR1PlayerController::TickCursorTrace()
{
	if (bMousePressed)
	{
		return;
	}

	FHitResult OutCursorHit;
	if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, OUT OutCursorHit) == false)
	{
		return;
	}

	AR1Character* LocalHighlightActor = Cast<AR1Character>(OutCursorHit.GetActor());
	if (LocalHighlightActor == nullptr)
	{
		if (HighlightActor)
		{
			HighlightActor->UnHighlight();
		}
	}
	else
	{
		if (HighlightActor)
		{
			if (HighlightActor != LocalHighlightActor)
			{
				HighlightActor->UnHighlight();
				LocalHighlightActor->Highlight();
			}
		}
		else
		{
			LocalHighlightActor->Highlight();
		}
	}

	HighlightActor = LocalHighlightActor;
}
```

---

## depth stencil hilight 처리

* PP_Highlight Material을 추가(Migrate)한다.
* Project Settings -> Engine -> Rendering -> Postprocessing을 Enabled with Stencil로 전환

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-23-1.png"/>
</p>

* 후 처리(Post Process)를 위해서 PostProcessVolume을 Level에 추가후 Inifinite Extent옵션을 켜준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-23-2.png"/>
</p>

* PostProcessVolume에 앞서 추가한 PP_Highlight Material을 넣어준다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-23-3.png"/>
</p>

* 이제 Monster의 Render CustomDepth Pass, CustomDepth Stencil Value 옵션에 따라 달라짐을 확인한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-23-4.png"/>
</p>

* 색상은 PP_Highlight Material에서 변경가능

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/grammer/ucpp0-23-5.png"/>
</p>

---

## 궁금한 점? depth stencil buffer는 여기서 왜 나오나?

* 정확히는 PostProcessing 과정에서 나왔다.
* PostProcessing에서 알다 싶이 depth stencil buffer를 통해 Object와 아닌 것의 구분을 한다.
* 그 구분값을 넣어 색을 조절한 것!