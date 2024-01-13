---
layout: default
title: "03. Title Scene 클릭 이벤트 처리"
parent: "(포트폴리오 4 - 방치형 게임)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## RayCast Target 처리

* 클릭되면 안될 부분의 Raycast Target을 Off시켜주자
    * Background, Display Text

---

```csharp
// Event Bind
public class UI_Base : InitBase
{
	// ...

	public static void BindEvent(GameObject go, Action<PointerEventData> action = null, Define.EUIEvent type = Define.EUIEvent.Click)
	{
		UI_EventHandler evt = Util.GetOrAddComponent<UI_EventHandler>(go);

		switch (type)
		{
			case Define.EUIEvent.Click:
				evt.OnClickHandler -= action;
				evt.OnClickHandler += action;
				break;
			case Define.EUIEvent.PointerDown:
				evt.OnPointerDownHandler -= action;
				evt.OnPointerDownHandler += action;
				break;
			case Define.EUIEvent.PointerUp:
				evt.OnPointerUpHandler -= action;
				evt.OnPointerUpHandler += action;
				break;
			case Define.EUIEvent.Drag:
				evt.OnDragHandler -= action;
				evt.OnDragHandler += action;
				break;
		}
	}
}
```

```csharp
public static class Extension
{
	// ...

	public static void BindEvent(this GameObject go, Action<PointerEventData> action = null, Define.EUIEvent type = Define.EUIEvent.Click)
	{
		UI_Base.BindEvent(go, action, type);
	}

    // ...
```

```csharp
public class UI_TitleScene : UI_Scene
{
    // ...

    public override bool Init()
    {
        if (base.Init() == false)
            return false;

        BindObjects(typeof(GameObjects));
        BindTexts(typeof(Texts));

        // 클릭시 이벤트 바인드
		GetObject((int)GameObjects.StartImage).BindEvent((evt) =>
		{
			Debug.Log("ChangeScene");
			Managers.Scene.LoadScene(EScene.GameScene);
		});

		GetObject((int)GameObjects.StartImage).gameObject.SetActive(false);
		GetText((int)Texts.DisplayText).text = $"";

		StartLoadAssets();

		return true;
    }
```