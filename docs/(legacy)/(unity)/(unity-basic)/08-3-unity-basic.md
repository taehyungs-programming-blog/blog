---
layout: default
title: "8-3. UI 자동화-1"
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

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/8.3)

* 개선하고자 하는 문제는 두 가지 이다.
    * Button의 OnClick 이벤트 처리를 Unity Tool(드래그 앤 드롭)을 이용해야한다.
    * UI_Button Script에 넘길 Text를 Unity Tool(SerializeField)을 이용해야한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-3-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-3-2.png"/>
</p>

---

```csharp
public class UI_Button : MonoBehaviour
{
    /*
        * 이름으로 gameObject를 찾는다. *
    */

    Dictionary<Type, UnityEngine.Object[]> _objects = new Dictionary<Type, UnityEngine.Object[]>();


    // * Button는 PointButton
    // * Text는 PointText, ScoreText라는 이름이 있을 경우 찾을 예정
    enum Buttons
    {
        PointButton,
    }

    enum Texts
    {
        PointText,
        ScoreText
    }

    private void Start()
    {
        Bind<Button>(typeof(Buttons));
        Bind<Text>(typeof(Texts));

        Get<Text>((int)Texts.ScoreText).text = "Bind Text Test!";
    }

    void Bind<T>(Type type) where T : UnityEngine.Object
    {
        string[] names = Enum.GetNames(type);

        UnityEngine.Object[] objects = new UnityEngine.Object[names.Length];
        _objects.Add(typeof(T), objects);   // 예를들어 Text라면 ( Text, [PointText,ScoreText] ) 이렇게 저장됨.

        for (int i = 0; i < names.Length; i++)
        {
            objects[i] = Util.FindChild<T>(gameObject,      // 현재 Unity에서 사용중인 모든 object의
                                            names[i], true);
        }
    }

    T Get<T>(int idx) where T : UnityEngine.Object
    {
        UnityEngine.Object[] objects = null;
        if (_objects.TryGetValue(typeof(T), out objects) == false)
            return null;

        return objects[idx] as T;
    }

    int _score = 0;

    public void OnButtonClicked()
    {
        _score++;
    }
}
```

```csharp
public class Util
{
    public static T FindChild<T>(GameObject go, string name = null, bool recursive = false) where T : UnityEngine.Object
    {
        if (go == null)
            return null;

        if(recursive == false)
        {
            for (int i = 0; i < go.transform.childCount; i++)
            {
                Transform transform = go.transform.GetChild(i);
                if (string.IsNullOrEmpty(name) || transform.name == name)
                {
                    T component = transform.GetComponent<T>();
                    if (component != null)
                        return component;
                }
            }
        }
        else 
        {
            foreach(T component in go.GetComponentsInChildren<T>())
            {
                if (string.IsNullOrEmpty(name) || component.name == name)
                    return component;
            }
        }

        return null;
    }
}
```

* 여기까지하면 `_objects`내에 찾고자 하는 object가 저장되어 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-3-3.png"/>
</p>

---

* 그런데 Text, Button말고 GameObject자체를 연결시킬 필요도 있다.
* 테스트를 위해 UI_Button Canvas안에 Empty Object를 하나 만들어 주자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-3-4.png"/>
</p>

```csharp
public class UI_Button : MonoBehaviour
{
    // ...

    enum GameObjects
    {
        TestObject,
    }

    private void Start()
    {
        Bind<Button>(typeof(Buttons));
        Bind<Text>(typeof(Texts));
        Bind<GameObject>(typeof(GameObjects));

        Get<Text>((int)Texts.ScoreText).text = "Bind Text Test!";
    }

    // ...
```

```csharp
void Bind<T>(Type type) where T : UnityEngine.Object
{
    string[] names = Enum.GetNames(type);

    UnityEngine.Object[] objects = new UnityEngine.Object[names.Length];
    _objects.Add(typeof(T), objects);

    for (int i = 0; i < names.Length; i++)
    {
        // Bind시 GameObject는 별도로 Bind
        if(typeof(T) == typeof(GameObject))
            objects[i] = Util.FindChild(gameObject, names[i], true);
        else
            objects[i] = Util.FindChild<T>(gameObject, names[i], true);

        if (objects[i] == null)
            Debug.Log($"Filed to bind{names[i]}");
    }
}
```

```csharp
public static GameObject FindChild(GameObject go, string name = null, bool recursive = false)
{
    // 찾는 방법은 모든 object는 Transform을 갖기에 Transform으로 찾고 gameObject를 리턴한다.
    Transform t = FindChild<Transform>(go, name, recursive);
    if (t != null)
        return null;

    return t.gameObject;
}
```

---

* 많이 사용되는 Text, Button은 별도의 함수로 빼두자.

```csharp
public class UI_Button : MonoBehaviour
{
    // ...
    Text GetText(int idx) { return Get<Text>(idx); }
    Button GetButton(int idx) { return Get<Button>(idx); }
    Image GetImage(int idx) { return Get<Image>(idx); }
```

---

* 현재 Bind가 모두 UI_Button 아래있는데 UI_Base아래로 옮기겠음.

```csharp
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UI_Button : UI_Base
{
    enum Buttons
    {
        PointButton,
    }
    enum Texts
    {
        PointText,
        ScoreText
    }
    enum GameObjects
    {
        TestObject,
    }

    private void Start()
    {
        Bind<Button>(typeof(Buttons));
        Bind<Text>(typeof(Texts));
        Bind<GameObject>(typeof(GameObjects));

        // Get<Text>((int)Texts.ScoreText).text = "Bind Text Test!";
        GetText((int)Texts.ScoreText).text = "Bind Text Test!";
    }



    int _score = 0;

    public void OnButtonClicked()
    {
        _score++;
    }
}

```

```csharp
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UI_Base : MonoBehaviour
{
    Dictionary<Type, UnityEngine.Object[]> _objects = new Dictionary<Type, UnityEngine.Object[]>();

    protected void Bind<T>(Type type) where T : UnityEngine.Object
    {
        string[] names = Enum.GetNames(type);

        UnityEngine.Object[] objects = new UnityEngine.Object[names.Length];
        _objects.Add(typeof(T), objects);

        for (int i = 0; i < names.Length; i++)
        {
            if (typeof(T) == typeof(GameObject))
                objects[i] = Util.FindChild(gameObject, names[i], true);
            else
                objects[i] = Util.FindChild<T>(gameObject, names[i], true);

            if (objects[i] == null)
                Debug.Log($"Filed to bind{names[i]}");
        }
    }

    protected T Get<T>(int idx) where T : UnityEngine.Object
    {
        UnityEngine.Object[] objects = null;
        if (_objects.TryGetValue(typeof(T), out objects) == false)
            return null;

        return objects[idx] as T;
    }

    protected Text GetText(int idx) { return Get<Text>(idx); }
    protected Button GetButton(int idx) { return Get<Button>(idx); }
    protected Image GetImage(int idx) { return Get<Image>(idx); }
}

```