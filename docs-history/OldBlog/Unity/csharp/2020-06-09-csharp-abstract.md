---
title: "Unity : 추상(abstract) 클래스"
permalink: unity/csharp/abstract/                # link 직접 지정
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
  - abstract
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class BaseMonster : MonoBehaviour
// interface와 달리 abstract는 일부의 구현물도(코드) 넣을 수 있다
{
    public float damage = 100f;

    private void Update()
    {
        if(Input.GetKeyDown(KeyCode.Space))
        {
            Attack();
        }
    }
    public abstract void Attack();
}

public class Oak : BaseMonster
{
    public override void Attack()
    {
        Debug.Log("한 캐릭터를 공격했다");
    }
}
```