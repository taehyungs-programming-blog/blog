---
title: "Unity : 프로퍼티 (getter, setter)"
permalink: unity/csharp/property/                # link 직접 지정
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
  - property
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PointManager : MonoBehaviour
{
    private int point = 0;

    public void SetPoint(int newPoint)
    {
        if (newPoint < 0) point = 0;
        else if (newPoint > 1000) point = 0;
        else point = newPoint;
    }

    public int GetPoint() { return point;  }

    // 매번 Set, Get함수를 써야하나??? -> 프로퍼티
}
```

---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PointManager : MonoBehaviour
{
    private int m_point = 0;
    public int point
    {
        get
        {
            return m_point;
        }
        set
        {
            if (value < 0) m_point = 0;
            else if (value > 1000) m_point = 0;
            else m_point = value;
        }
    }
}
```

```csharp
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class MonsterManager : MonoBehaviour
{
    public int count
    {
        get
        {
            Monster[] monsters = FindObjectsOfType<Monster>();
            return monsters.Count;
        }
    }
}
```