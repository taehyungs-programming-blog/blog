---
title: "C# : Singleton Pattern"
permalink: unity/csharp/singleton/                # link 직접 지정
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

public class Ninja : MonoBehaviour
{
    // Start is called before the first frame update
    // 대략적인 Singleton의 개념
    public static Ninja ninjaKing;

    public string ninjaName;
    public bool isKing;

    void Start()
    {
        if(isKing)
        {
            ninjaKing = this;
        }
    }

    // Update is called once per frame
    void Update()
    {
        Debug.Log("MyName : " + ninjaName);
        Debug.Log("Who is NinJaKing : " + Ninja.ninjaKing.ninjaName);
    }
}
```

![](/file/image/csharp-singleton-1.png)

매번 저렇게 해야하나?

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScoreManager : MonoBehaviour
{
    // Start is called before the first frame update
    public static ScoreManager instance;
    private int score = 0;

    private void Awake()
    {
        instance = this;
    }

    public int GetScore()
    {
        return score;
    }

    public void AddScore(int newScore)
    {
        score = newScore;
    }

    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
```

---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/*
// 싱클턴을 선언하지 않은 시절
public class ScoreAdder : MonoBehaviour
{
// Start is called before the first frame update
public ScoreManager scoreManager;
void Start()
{

}

// Update is called once per frame
void Update()
{
    // 왼쪽마우스 클릭
    if(Input.GetMouseButtonDown(0))
    {
        scoreManager.AddScore(5);
        Debug.Log("Score : " + scoreManager.GetScore());
    }
}
}
*/

public class ScoreAdder : MonoBehaviour
{
// Start is called before the first frame update
void Start()
{

}

// Update is called once per frame
void Update()
{
    // 왼쪽마우스 클릭
    if (Input.GetMouseButtonDown(0))
    {
        ScoreManager.instance.AddScore(5);
        Debug.Log("Score : " + ScoreManager.instance.GetScore());
    }
}
}
```

---

## 최종, 싱글톤 고도화

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScoreManager : MonoBehaviour
{
// Start is called before the first frame update
public static ScoreManager GetInstance()
{
if(instance == null)
{
instance = FindObjectOfType<ScoreManager>();
// Tips. FindObjectOfType는 성능의 문제가 많기에 자주 호출하면 안된다.

        // 이런식으로 지연생성(사용할때 생성)이 가능하다
        if(instance == null)
        {
            GameObject container = new GameObject("ScoreManager");
            instance = container.AddComponent<ScoreManager>();
        }
    }

    return instance;
}

private static ScoreManager instance;
private int score = 0;

public int GetScore()
{
    return score;
}

public void AddScore(int newScore)
{
    score = newScore;
}

void Start()
{
    if(instance != null)
    {
        if(instance != this)
        {
            // Singleton이 하나가 아니다
            Destroy(gameObject);    // 자신을 제거한다.
        }
    }
}

// Update is called once per frame
void Update()
{
    
}
}
```