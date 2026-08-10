---
layout: default
title: "1. 환경설정"
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

## 환경

* Visual Studio 2019
* Unity 2019.4.xx

---

## 프로젝트 생성

* 3D로 프로젝트 생성하고 특별히 바꿀것은 없다.
* 레이아웃 정도를 아래와 같이(2 by 3) 수정하자.
    * Menu -> Window -> Layout -> 2 by 3

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-1-1.png"/>
</p>

---

## Hello World 출력

* 새로운 object 생성 👉 Hierachy -> Create Empty
* 새로운 script 생성 👉 Project -> Assets아래 -> Create -> C# Script
* 생성한 Empty Object아래 Script를 드래그 앤 드롭 한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-1-2.png"/>
</p>

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HelloUnity : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("Hello Unity");
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
```

* Ctrl + Shift + c -> 콘솔창 출력
    * 혹은 Window -> General -> Console

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-1-3.png"/>
</p>