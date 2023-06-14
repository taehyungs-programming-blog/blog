---
title: "C# : list"
permalink: unity/csharp/list/                # link 직접 지정
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
  - list
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEditor.Build.Player;
using UnityEngine;

public class ScoreManager : MonoBehaviour
{
    public int[] socres = new int[10];          // 배열은 다 좋은데 배열이 늘어날때 값을 복사해줘야 한다.
    public List<int> listScores = new List<int>(); // 그래서 List를 써보자.
    private int index = 0;
    // Start is called before the first frame update
    void Start()
    {

    }
    // Update is called once per frame
    void Update()
    {
        // 클릭을 할때마다 랜덤으로 값을 넣는다.
        if(Input.GetMouseButtonDown(0))
        {
            socres[index++] = Random.Range(0, 100);
            int randomNum = Random.Range(0, 100);
            listScores.Add(randomNum);          // List는 값을 어디 넣을지 지정하지 않아도 됨.
        }

        // 마우스 우클릭
        if (Input.GetMouseButtonDown(1))
        {
            listScores.RemoveAt(3);
        }
    }
}
```