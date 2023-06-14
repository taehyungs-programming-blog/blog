---
title: "C# : 오버로드"
permalink: unity/csharp/overload/                # link 직접 지정
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-07 00:00:00 -0000
last_modified_at: 2020-06-07 00:00:00 -0000
sidebar:
  title: "unity" 
  nav: unity
tag:
  - unity
  - C#
category:
  - overload
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Calc : MonoBehaviour
{
    private void Start()
    {
        Debug.Log("Sum2 : " + Sum(1, 1));
        Debug.Log("Sum3 : " + Sum(1, 1, 2));
    }
    public int Sum(int a, int b)
    {
        return a + b;
    }

    // 오버로드 이름은 똑갖지만 매개변수가 다르다
    public int Sum(int a, int b, int c)
    {
        return a + b + c;
    }
}
```