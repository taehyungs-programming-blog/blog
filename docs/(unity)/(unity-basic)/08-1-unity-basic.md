---
layout: default
title: "8-1. UI 추가해 보기"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/8.1)

* 우클릭 UI -> Canvas 추가
* Canvas아래 Button 추가

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-1-1.png"/>
</p>

* 화면을 보기 쉽게 2D로 변경

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-1-2.png"/>
</p>

* UI의 경우 환경에 따라 그려지는 부분이 달라져야할 경우가 있다
* 예를들어 핸드폰에서 그려질 UI와 컴퓨터 화면에서 그려질 UI는 달라야 하는데 그 부분을 어떻게 처리하면 좋을까?
    * 👉 앵커를 이용해 처리한다
* 앵커는 말로 설명하면 어렵고 아래 그림의 바람개비 같은 이미지이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-1-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-1-4.png"/>
</p>

* 앵커의 동작은 앵커와 부모의 거리비율을 구해 부모의 크기가 변해도 항상 그 **비율**을 유지해 주며,
* 앵커와 자식의 거리를 구해 앵커대비 자식의 UI위치를 항상 **일정(상수)**하게 유지해준다.
* 사실 말로 설명하면 어렵고 그냥 Unity에서 만저보면 이해된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-1-5.png"/>
</p>

* 보통은 앵커와 자녀의 사이즈를 맞춰서 화면의 사이즈가 바뀌면 UI도 같이 변경되게 한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-8-1-6.png"/>
</p>