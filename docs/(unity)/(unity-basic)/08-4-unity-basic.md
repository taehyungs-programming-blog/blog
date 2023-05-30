---
layout: default
title: "8-4. UI 자동화-2"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/8.4)

* Event처리를 해보자.
* Event 처리용 C# Script 생성

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-4-1.png"/>
</p>

* 하고싶은것은 UI_Button안의 Image를 Drag-Drop 처리가 가능하게 변경하는 것이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-4-2.png"/>
</p>

* 우선, UI_EventHandler를 UI_Button의 Image object에 연결시키자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-4-3.png"/>
</p>

```csharp
public class UI_EventHandler : MonoBehaviour, IBeginDragHandler, IDragHandler
{
    public void OnBeginDrag(PointerEventData eventData)
    {
        Debug.Log("OnBeginDrag");
    }

    public void OnDrag(PointerEventData eventData)
    {
        Debug.Log("OnDrag");
    }
}
```

* Drag처리가 되는 것 까지 확인가능하다.
* 이제 Image를 움직여 보자.

```csharp
public class UI_EventHandler : MonoBehaviour, IBeginDragHandler, IDragHandler
{
    public void OnBeginDrag(PointerEventData eventData)
    {
        Debug.Log("OnBeginDrag");
    }

    public void OnDrag(PointerEventData eventData)
    {
        transform.position = eventData.position;
        Debug.Log("OnDrag");
    }
}
```

---

* Drag Event를 받았음을 다른 곳에도 알리고 싶다.

```csharp
public class UI_EventHandler : MonoBehaviour, IBeginDragHandler, IDragHandler
{
    public Action<PointerEventData> OnBeginDragHandler = null;
    public Action<PointerEventData> OnDragHandler = null;

    public void OnBeginDrag(PointerEventData eventData)
    {
        //Debug.Log("OnBeginDrag");

        if (OnBeginDragHandler != null)
            OnBeginDragHandler.Invoke(eventData);
    }

    public void OnDrag(PointerEventData eventData)
    {
        //transform.position = eventData.position;
        //Debug.Log("OnDrag");

        if (OnDragHandler != null)
            OnDragHandler.Invoke(eventData);
    }
}
```

```csharp
public class UI_Button : UI_Base
{
    // ...

    enum Images
    {
        IconImage,
    }

    private void Start()
    {
        Bind<Button>(typeof(Buttons));
        Bind<Text>(typeof(Texts));
        Bind<GameObject>(typeof(GameObjects));
        Bind<Image>(typeof(Images));

        // Get<Text>((int)Texts.ScoreText).text = "Bind Text Test!";
        GetText((int)Texts.ScoreText).text = "Bind Text Test!";

        GameObject go = GetImage((int)Images.IconImage).gameObject;
        UI_EventHandler evt = go.GetComponent<UI_EventHandler>();
        evt.OnBeginDragHandler += ((PointerEventData data) => { evt.gameObject.transform.position = data.position; });
    }
```

---

* Handler 코드를 조금 정리해 보자.

```csharp
public class UI_Button : UI_Base
{
    // ...

    enum Images
    {
        IconImage,
    }

    private void Start()
    {
        Bind<Button>(typeof(Buttons));
        Bind<Text>(typeof(Texts));
        Bind<GameObject>(typeof(GameObjects));
        Bind<Image>(typeof(Images));

        // Get<Text>((int)Texts.ScoreText).text = "Bind Text Test!";
        GetText((int)Texts.ScoreText).text = "Bind Text Test!";

        GameObject go = GetImage((int)Images.IconImage).gameObject;
        AddUIEvnet(go,
            (PointerEventData data) => { go.transform.position = data.position; },
            Define.UIEvent.Drag);
    }
```

```csharp
public class UI_Base : MonoBehaviour
{
    // ...

    public static void AddUIEvnet(GameObject go, Action<PointerEventData> action, Define.UIEvent type = Define.UIEvent.Click)
    {
        UI_EventHandler evt = Util.GetOrAddComponent<UI_EventHandler>(go);

        switch(type)
        {
            case Define.UIEvent.Click:
                evt.OnClickHandler -= action;
                evt.OnClickHandler += action;
                break;
            case Define.UIEvent.Drag:
                evt.OnDragHandler -= action;
                evt.OnDragHandler += action;
                break;
        }
    }
}

```

```csharp
public class Util
{
    public static T GetOrAddComponent<T>(GameObject go) where T : UnityEngine.Component
    {
        T component = go.GetComponent<T>();
        if (component == null)
            component = go.AddComponent<T>();

        return component;
    }

    // ...
```

```csharp
public class Define
{
    public enum UIEvent
    {
        Click,
        Drag,
    }
```

---

* Extension으로 좀 더 코드를 간략화 해보자.
* C# Script Extension을 생성

```csharp
public static class Extension
{
    public static void AddUIEvnet(this GameObject go, Action<PointerEventData> action, Define.UIEvent type = Define.UIEvent.Click)
    {
        UI_Base.AddUIEvnet(go, action, type);
    }
}
```

```csharp
public class UI_Button : UI_Base
{
    // ...

    private void Start()
    {
        Bind<Button>(typeof(Buttons));
        Bind<Text>(typeof(Texts));
        Bind<GameObject>(typeof(GameObjects));
        Bind<Image>(typeof(Images));

        // Get<Text>((int)Texts.ScoreText).text = "Bind Text Test!";
        GetText((int)Texts.ScoreText).text = "Bind Text Test!";

        GameObject go = GetImage((int)Images.IconImage).gameObject;
        AddUIEvnet(go,
            (PointerEventData data) => { go.transform.position = data.position; },
            Define.UIEvent.Drag);

        // Extension 활용
        GetButton((int)Buttons.PointButton).gameObject.AddUIEvnet(OnButtonClicked);
    }
```