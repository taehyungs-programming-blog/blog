---
layout: default
title: "7-6. Animation Event"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/7.6)

* Animation 우클릭 -> Add Animation Event
* 뭔진 모르겠으나 파란짝대기가 생긴다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-6-1.png"/>
</p>

* Animation의 특정시점에서 Callback을 받길 원할경우 사용된다.
* Event를 받을 C# Script를 하나 생성하자 -> CubeEventTest
* 내가 사용할 Cube에 C# Script를 붙이고

```csharp
public class CubeEventTest : MonoBehaviour
{
    void TestEventCallback()
    {
        Debug.Log("callback!");
    }
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-6-2.png"/>
</p>

---

* 기존의 Animation으로 적용해보자
* unity-chan의 walk Animation에 Event를 붙여보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-6-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-6-4.png"/>
</p>

```csharp
public class PlayerController : MonoBehaviour
{
	// ...

	void OnRunEvent(int n)
    {
		Debug.Log($"OnRunEvent {n}");
    }
```