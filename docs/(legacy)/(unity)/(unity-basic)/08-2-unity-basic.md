---
layout: default
title: "8-2. Button Event"
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

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/8.2)

* Button을 클릭시 Text를 변경되게 해보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-2-1.png"/>
</p>

* Button의 Inspector에 가보면 On Click()이라고 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-2-2.png"/>
</p>

* UI_Button C# Script를 만들어서 OnClick Event를 받도록 해보자
* UI Canvas역시 Prefab으로 저장하고 필요할때마다 저장된 Prefab을 가져오는 식으로 구현이 가능하다.
* 현재 구현된 UI Canvas도 Prefab으로 만들어 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-2-3.png"/>
</p>

* On Click을 object를 UI_Button Canvas로 지정하고

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-2-4.png"/>
</p>

* On Click을 받아줄 Script를 작성하자

```csharp
public class UI_Button : MonoBehaviour
{
    public void OnButtonClicked()
    {
        Debug.Log("ButtonClicked");
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-2-5.png"/>
</p>

* Button 클릭 이벤트를 받는 경우는 Button 이벤트만 처리하지 캐릭의 동작은 안되게 바꿔놔야한다.

```csharp
public class InputManager
{
    public Action KeyAction = null;
    public Action<Define.MouseEvent> MouseAction = null;

    bool _pressed = false;

    public void OnUpdate()
    {
        // UI 가 클릭되었는지 확인
        if (EventSystem.current.IsPointerOverGameObject())
            return;

        // ...
```

* 이제 Text를 업데이트 시켜보자.

```csharp
public class UI_Button : MonoBehaviour
{
    [SerializeField]
    Text _text;

    int _score = 0;

    public void OnButtonClicked()
    {
        // Debug.Log("ButtonClicked");

        _score++;
        _text.text = $"점수 : {_score}";
    }
}
```

* 이제 `_text`를 UI Text에 연결시키자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-2-6.png"/>
</p>

---

### UI Canvas 자체를 불러오는 것을 해보자.

```csharp
public class PlayerController : MonoBehaviour
{
    [SerializeField]
    float _speed = 10.0f;

	Vector3 _destPos;

    void Start()
    {
		Managers.Input.MouseAction -= OnMouseClicked;
		Managers.Input.MouseAction += OnMouseClicked;

        // 시작과 동시에 UI를 불러온다.
		Managers.Resource.Instantiate("UI/UI_Button");
	}

    // ...
```

---

* 개선사항이 몇 가지 있다.
    * UI를 Unity Tool로 하나하나 연결하는 것은 너무 큰 일이다.
    * **자동화(UI Manager)**가 필요해 보인다. 
