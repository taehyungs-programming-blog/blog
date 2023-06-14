---
title: "Unity : interface"
permalink: unity/csharp/interface/                # link 직접 지정
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
  - interface
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class cs : MonoBehaviour
{
    // 외부 오브젝트에 접근할때 매번 이런식으로 접근해야 하나?
    private void OnTriggerEnter2D(Collider2D other)
    {
    GoldItem goldItem = other.GetComponent<GoldItem>();

        if(goldItem != null)
        {
            goldItem.Use();
        }

        HealthKitItem healthKit = other.GetComponent<HealthKitItem>();

        if(healthKit != null)
        {
            healthKit.Use();
        }

        // 만약 확인해야할 오브젝트가 100개라면 if문이 100개가 될것인가?
    }
}
```

인터페이스를 활용해보자.

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public interface IItem
{
    void Use();
}

public class GoldItem : MonoBehaviour, IItem
{
    public void Use()
    {
        // Do something
    }
}

public class HealthKitItem : MonoBehaviour, IItem
{
    public void Use()
    {
        // Do something
    }
}
```

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class cs : MonoBehaviour
{
    private void OnTriggerEnter2D(Collider2D collision)
    {
        IItem item = collision.GetComponent<IItem>();

        if(item != null)
            item.Use();          // so good
    }
}
```