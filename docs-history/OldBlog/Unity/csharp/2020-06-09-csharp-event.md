---
title: "Unity : event"
permalink: unity/csharp/event/                # link 직접 지정
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
  - event
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Creator : MonoBehaviour
{
    public Booster booster;     // Booster를 직접알아야 하나?

    public string playerName = "Jemin";
    public float hp = 100;
    public float defense = 50;
    public float damage = 30;

    void Awake()
    {
        booster.HealthBoost(this);
    }
}
```

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Charater : MonoBehaviour
{
    public event void Boost(Charater target);

    public Boost playerBoost;

    public string playerName = "Jemin";
    public float hp = 100;
    public float defense = 50;
    public float damage = 30;

    void Start()
    {
        playerBoost(this);
    }
}

public class Booster : MonoBehaviour
{
    public void HealthBoost()
    {

    }

    public void Health2Boost()
    {
    }

    private void Awake()
    {
        Charater player = FindObjectOfType<Charater>();

        // 이런식으로 이벤트 처리가 가능해 진다.
        player.playerBoost += HealthBoost;
            player.playerBoost = HealthBoost; // 기존에 처리중인 event가 있다면 error가 나타난다
    }
}
```