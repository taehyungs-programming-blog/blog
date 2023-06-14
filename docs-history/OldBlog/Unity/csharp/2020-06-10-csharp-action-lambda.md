---
title: "Unity : 액션, 람다 함수"
permalink: unity/csharp/action-lambda/                # link 직접 지정
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-10 00:00:00 -0000
last_modified_at: 2020-06-10 00:00:00 -0000
sidebar:
  title: "unity" 
  nav: unity
tag:
  - unity
  - C#
category:
  - action
  - lambda
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

## Action

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Worker : MonoBehaviour
{
    // 아래와 같이 리턴이 void 입력이 void인 delegate는 C#이 더 편하게 생성하게 도와준다
    delegate void Work();

    Work work;

    void MoveBricks()
    {
        Debug.Log("벽돌");
    }

    void DigIn()
    {
        Debug.Log("땅");
    }

    void Start()
    {
        work += MoveBricks;
        work += DigIn;
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetKeyDown(KeyCode.Space))
        {
            work();
        }
    }
}
```

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;       // for Action

public class Worker : MonoBehaviour
{
    // delegate void Action()
    Action work;

    void MoveBricks()
    {
        Debug.Log("벽돌");
    }

    void DigIn()
    {
        Debug.Log("땅");
    }

    void Start()
    {
        work += MoveBricks;
        work += DigIn;
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetKeyDown(KeyCode.Space))
        {
            work();
        }
    }
}
```

---

## Lambda

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Messanger : MonoBehaviour
{
    public delegate void Send(string reciever);
    Send onSend;

    // 람다 표현으로 간단하게 해보자
    void SendMail(string reciever)
    {
        Debug.Log("Send");
    }

    void SendMoney(string reciever)
    {
        Debug.Log("Send");
    }
    // Start is called before the first frame update
    void Start()
    {
        onSend += SendMail;
        onSend += SendMoney;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            onSend("hello");
        }
    }
}
```

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Messanger : MonoBehaviour
{
    public delegate void Send(string reciever);
    Send onSend;

    // 람다 표현으로 간단하게 해보자
    void SendMail(string reciever)
    {
        Debug.Log("Send");
    }

    void SendMoney(string reciever)
    {
        Debug.Log("Send");
    }
    // Start is called before the first frame update
    void Start()
    {
        onSend += SendMail;
        onSend += SendMoney;
        
        // 람다 표현 (매개변수 => 함수내부)
        onSend += man => Debug.Log("Assainate " + man);
        onSend += (string man) => { Debug.Log("Assainate2 " + man); Debug.Log("Assainate3 " + man); }
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            onSend("hello");
        }
    }
}
```