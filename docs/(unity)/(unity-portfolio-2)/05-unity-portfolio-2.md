---
layout: default
title: "5. 분석"
parent: "(포트폴리오 2 - 클리커 게임)"
grand_parent: "(Unity ✨)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 시작화면

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unity/portfolio-2/p2-5-1.png"/>
</p>

* 우선 이 Scene은 어떻게 로드되나?
* 간단, 그냥 @Scene을 GameScene에 넣어둠

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unity/portfolio-2/p2-5-2.png"/>
</p>

```csharp
// 게임 시작시 GameScene이 로드 될 것이다.
public class GameScene : BaseScene
{
    protected override bool Init()
    {
		if (base.Init() == false)
			return false;

        SceneType = Define.Scene.Game;

        // 결국 UI호출은 여기서
		Managers.UI.ShowPopupUI<UI_TitlePopup>();
		Debug.Log("Init");
		return true;
	}
}
```

```csharp
// UIManager

public T ShowPopupUI<T>(string name = null, Transform parent = null) where T : UI_Popup
{
    if (string.IsNullOrEmpty(name))
        name = typeof(T).Name;

    GameObject prefab = Managers.Resource.Load<GameObject>($"Prefabs/UI/Popup/{name}");

    GameObject go = Managers.Resource.Instantiate($"UI/Popup/{name}");
    T popup = Utils.GetOrAddComponent<T>(go);
    _popupStack.Push(popup);

    if (parent != null)
        go.transform.SetParent(parent);
    else if (SceneUI != null)
        go.transform.SetParent(SceneUI.transform);
    else
        go.transform.SetParent(Root.transform);

    go.transform.localScale = Vector3.one;
    go.transform.localPosition = prefab.transform.position;

    return popup;
}
```

* 여기까지하면 @UI_Root\UI_TitlePopup이 생성된다.

---

## 시작화면 버튼

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unity/portfolio-2/p2-5-3.png"/>
</p>

```csharp
// Button Bind는 이렇게

public override bool Init()
{
    if (base.Init() == false)
        return false;

    BindText(typeof(Texts));
    BindButton(typeof(Buttons)); 

    GetButton((int)Buttons.StartButton).gameObject.BindEvent(OnClickStartButton);
    GetButton((int)Buttons.ContinueButton).gameObject.BindEvent(OnClickContinueButton);
    GetButton((int)Buttons.CollectionButton).gameObject.BindEvent(OnClickCollectionButton);


    GetText((int)Texts.StartButtonText).text = Managers.GetText(Define.StartButtonText);
    GetText((int)Texts.ContinueButtonText).text = Managers.GetText(Define.ContinueButtonText);
    GetText((int)Texts.CollectionButtonText).text = Managers.GetText(Define.CollectionButtonText);

    Managers.Sound.Clear();
    Managers.Sound.Play(Sound.Effect, "Sound_MainTitle");
    return true;
}
```

```csharp
// 특이하다 할만한 점은, Bind 이다

enum Buttons
{
    StartButton,
    ContinueButton,
    CollectionButton
}
```

```csharp
// ...

protected void BindButton(Type type) { Bind<Button>(type);  }

// ...

protected void Bind<T>(Type type) where T : UnityEngine.Object
{
    string[] names = Enum.GetNames(type);
    UnityEngine.Object[] objects = new UnityEngine.Object[names.Length];
    _objects.Add(typeof(T), objects);

    for (int i = 0; i < names.Length; i++)
    {
        if (typeof(T) == typeof(GameObject))
            objects[i] = Utils.FindChild(gameObject, names[i], true);
        else
            objects[i] = Utils.FindChild<T>(gameObject, names[i], true);

        if (objects[i] == null)
            Debug.Log($"Failed to bind({names[i]})");
    }
}
```

* Bind한 Button의 사용은

```csharp
GetButton((int)Buttons.StartButton).gameObject.BindEvent(OnClickStartButton);
```

```csharp
protected Button GetButton(int idx) { return Get<Button>(idx); }

// ...

protected T Get<T>(int idx) where T : UnityEngine.Object
{
    UnityEngine.Object[] objects = null;
    if (_objects.TryGetValue(typeof(T), out objects) == false)
        return null;

    return objects[idx] as T;
}
```

* 나머지는 뭐 그리 특별할것이 없다