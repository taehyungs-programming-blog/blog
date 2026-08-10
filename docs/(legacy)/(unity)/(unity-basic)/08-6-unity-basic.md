---
layout: default
title: "8-6. 인벤토리 만들기"
parent: "(Basic)"
grand_parent: "(Unity ✨)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/8.6.1)

## 사전사항

* [에셋스토어](https://assetstore.unity.com/) -> [Unity Samples UI](https://assetstore.unity.com/packages/essentials/ui-samples-25468)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-1.png"/>
</p>

* Texture and Sprites만 import 하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-2.png"/>
</p>

* Window -> Package Manager -> 2D Sprite 설치

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-3.png"/>
</p>

* 이제 Sprite Editor를 사용할 수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-4.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-5.png"/>
</p>

---

## 인벤토리 만들기

* 우클릭 -> UI -> Panel

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-6.png"/>
</p>

* Unity Samples UI의 Rouned UI / UIPanel을 Source Image로 넣는다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-7.png"/>
</p>

* Panel내부에 Panel을 넣고 위 과정을 반복

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-8.png"/>
</p>

* 만약 UI가 좀 흐리다면 Pixels Per Unit 수치를 올리도록 하자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-9.png"/>
</p>

* Panel 내에 Image를 넣고 Decoration Image를 추가해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-10.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-11.png"/>
</p>

* UI_Inven_Item을 Prefab으로 만들어 둔다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-12.png"/>
</p>

* UI_Inven_Item을 여러개 넣을 것이기에 Panel에 Layout Component를 추가하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-13.png"/>
</p>

---

## 인벤토리 만들기 2차

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/8.6.2)

* UI 관리용 Script를 만들자.

```csharp
public class UI_Inven : UI_Scene
{
    enum GameObjects
    {
        GridPanel
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        base.Init();

        Bind<GameObject>(typeof(GameObjects));
        
        // 기존의 Item을 날린다.
        GameObject gridPanel = Get<GameObject>((int)GameObjects.GridPanel);
        foreach (Transform child in gridPanel.transform)
            Managers.Resource.Destroy(child.gameObject);

        // Item 을 8개 넣어보자.
        for(int i = 0; i <8; i++)
        {
            GameObject item = Managers.Resource.Instantiate("UI/Scene/UI_Inven_Item");
            item.transform.SetParent(gridPanel.transform);
        }
    }
}
```

```csharp
public class PlayerController : MonoBehaviour
{
    // ....

    void Start()
    {
		Managers.Input.MouseAction -= OnMouseClicked;
		Managers.Input.MouseAction += OnMouseClicked;

		// Temp Inven
		Managers.UI.ShowSceneUI<UI_Inven>();
	}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-14.png"/>
</p>

* 아이템의 이름을 동적으로 변경

```csharp
public class UI_Inven_Item : UI_Base
{
    enum GameObjects
    {
        ItemIcon,
        ItemNameText,
    }

    void Start()
    {
        Init();
    }

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ItemNameText).GetComponent<Text>().text = "바인드";
    }

    // ...
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-6-15.png"/>
</p>

* Item의 클릭이벤트를 받도록 처리해 보자.

```csharp
public class UI_Inven_Item : UI_Base
{
    // ...

    public override void Init()
    {
        Bind<GameObject>(typeof(GameObjects));

        Get<GameObject>((int)GameObjects.ItemNameText).GetComponent<Text>().text = _name;

        Get<GameObject>((int)GameObjects.ItemImage).AddUIEvnet((PointerEventData) => { Debug.Log($"아이템 클릭 {_name}"); });
    }
```

---

## 최종 정리된 코드

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/8.6.3)