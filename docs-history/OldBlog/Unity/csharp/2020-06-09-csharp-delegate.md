---
title: "Unity : delegate"
permalink: unity/csharp/delegate/                # link 직접 지정
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-09 00:00:00 -0000
last_modified_at: 2020-06-09 00:00:00 -0000
sidebar:
  title: "unity" 
  nav: unity
tag:
  - unity
  - C#
category:
  - delegate
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Calculator : MonoBehaviour
{
public float Sum(float a, float b)
{
    Debug.Log(a + b);
    return a + b;
}

public float Sub(float a, float b)
{
    Debug.Log(a - b);
    return a - b;
}

public float Mul(float a, float b)
{
    Debug.Log(a * b);
    return a * b;
}

private void Update()
{
    if(Input.GetKeyDown(KeyCode.Space))
    {
        // 이걸 매번 실행시점에 다 알고 있어야 하나??
        
        Sum(1, 10);
        Sub(1, 10);
        Mul(1, 10);
        

        // 델리게이트를 써보자.

    }
}
}
```

---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Calculator : MonoBehaviour
{
    // 델리게이트의 정의
    delegate float Calculate(float a, float b);
    Calculate onCalculate;

    private void Start()
    {
        onCalculate = Sum;      // 주소값을 넘긴다

        onCalculate += Sub;    // 이것도 동작함. 2개의 함수가 돌아감
        onCalculate -= Sub;     // ok 빼기도 된다.
    }

    public float Sum(float a, float b)
    {
        Debug.Log(a + b);
        return a + b;
    }

    public float Sub(float a, float b)
    {
        Debug.Log(a - b);
        return a - b;
    }

    public float Mul(float a, float b)
    {
        Debug.Log(a * b);
        return a * b;
    }

    private void Update()
    {
        if(Input.GetKeyDown(KeyCode.Space))
        {
            onCalculate(1, 10);
        }
    }
}
```