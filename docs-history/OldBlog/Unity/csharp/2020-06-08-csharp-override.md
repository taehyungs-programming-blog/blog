---
title: "Unity : 오버라이드"
permalink: unity/csharp/override/                # link 직접 지정
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-08 00:00:00 -0000
last_modified_at: 2020-06-08 00:00:00 -0000
sidebar:
  title: "unity" 
  nav: unity
tag:
  - unity
category:
  - override
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BaseRotator : MonoBehaviour
{
public float speed = 60f;

private void Update()
{
    Rotate();
}
protected void Rotate()
{
    // 문제 - 방향마다 이걸 만들어야하나?
    transform.Rotate(speed * Time.deltaTime, 0, 0);
    // 이 구현을 자식에서 해보자.
}
}
```

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BaseRotator : MonoBehaviour
{
public float speed = 60f;

private void Update()
{
    Rotate();
}
protected virtual void Rotate()
{
    // transform.Rotate(speed * Time.deltaTime, 0, 0);
}
}
```

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class xRotator : BaseRotator
{
    protected override void Rotate()
    {
        base.Rotate();
        transform.Rotate(speed * Time.deltaTime, 0, 0);
    }
}
```