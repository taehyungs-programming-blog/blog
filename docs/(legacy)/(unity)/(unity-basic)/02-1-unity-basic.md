---
layout: default
title: "2. Manager와 Singleton 만들기"
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

* [Get Code 🌍]()

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HelloUnity : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        // x, y, z축을 1도씩 돌린다.
        transform.Rotate(new Vector3(1.0f, 1.0f, 1.0f));
    }
}
```

---

## Manager 만들기

* 네트워크나 UI등의 데이터를 관리할 Manager 클래스를 만들고자 한다.
* 그냥 C# 스크립를 Manager란 이름으로 만들어 넣으면 될까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-2-1.png"/>
</p>

```csharp
public class Players : MonoBehaviour
{
    void Start()
    {
        Managers m = new Managers();    // Error - MonoBehaviour로 생성된 Manager를 new할수 없음.
        // 이런식으로 부르지 말고 unity object를 만들어서 불러야 한다.
    }

    // ...
```

```csharp
public class Players : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        GameObject o = GameObject.Find("@Manager");
        Managers m = o.GetComponent<Managers>();
    }

    // ...
```

* 그런데 이런 실수를 할 수 있다. 실수로 @Manager를 하나 더 생성해 버렸다면??

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-2-2.png"/>
</p>

* 두 개의 Manager가 생성되어 버린다.
* 강제로 하나만 사용하게 해보자.

---

## Singleton

```csharp
public class Managers : MonoBehaviour
{
    static Managers Instance;
    public static Managers GetInstance() { Init(); return Instance; }

    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    // Init 시점에 만듦을 기억.
    static void Init()
    {
        if (Instance == null)
        {
            GameObject o = GameObject.Find("@Manager");
            if (o == null)  // @Manager가 없는 상황을 대비
            {
                // GameObject에 Manager를 붙여야함을 기억하자.
                o = new GameObject { name = "@Manager" };
                o.AddComponent<Managers>();
            }

            DontDestroyOnLoad(o);   // 절대 삭제하지마라
            Instance = o.GetComponent<Managers>();
        }
        
    }
}
```

```csharp
public class Players : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Managers m = Managers.GetInstance();
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
```

---

* 코드정리

```csharp
public class Managers  MonoBehaviour
{
    static Manager s_instance;
    public static Managers instance { get { Init(); return s_instance; } }

    void Start()
    {
        Init();
    }

    // ... 상동
}
```
