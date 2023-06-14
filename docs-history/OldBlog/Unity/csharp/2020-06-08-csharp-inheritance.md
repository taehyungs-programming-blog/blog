---
title: "Unity : 상속"
permalink: unity/csharp/inheritance/                # link 직접 지정
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
  - inheritance
excerpt: ""
header:
  teaser: /file/image/unity-page-teaser.gif
---

```csharp
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Animal
{
    public string name;
    public float weight;
    public int year;

    public void Print()
    {
        Debug.Log(name + " | 몸무게 : " + weight + " | 나이 : " + year);
    }

    public float GetSpeed()
    {
        float speed = 100f / (weight * year);
        return speed;
    }
}

public class Dog : Animal
{
    public void Hunt()
    {
    float speed = GetSpeed();   // 이런식으로 불러와서 사용가능.
    Debug.Log(speed + "의 속도로 사냥");

        weight += 10f;
    }
    }

public class Cat : Animal
{
    public void Stealth()
    {
        Debug.Log("숨었다");
    }
}
```

```csharp
// 다형성
Dog nate = new Dog();
Cat jack = new Cat();
Animal someAnimal = jack;       // ok
someAnimal.Hunt();              // error - Dog의 기능은 사용할 수 없다

Dog myDog = (Dog)someAnimal;    // ok

// 어디다 쓸까?
// 공통분모로 여러 오브젝트 관리가 편하다
Animal[] anmials = new Animal[2];
animals[0] = jack;
animals[1] = nate;

for(int i = 0; i < animals.Length; i++)
{
// ... do something
}
```