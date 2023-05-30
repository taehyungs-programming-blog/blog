---
layout: default
title: "7-1. Animation 적용"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍]()

---

## 애니메이션 파일 보는 방법

* 다운받은 Unity Chan 내의 Animations를 보자
* Inspector -> Animation
  * 왜 아래 그림처럼 안나오지 하면 아래 애니메이션 파일 이름을 드래그해서 위로 올려야 함(잘볼것)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-1.png"/>
</p>

* 그냥 보면 느낌이 안사니 Unity-Chan을 드래그 드롭하면

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-2.png"/>
</p>

* 자! 이제 Unity Chan에 Animation을 넣어보자.
  * 주의할 점은 모든 Model에 Animation을 넣을 순 없다.
  * Animation Type에 맞게 넣을순 있다.(여기선 Humanoid임, 말인즉슨 Mumanoid는 모두 적용가능한 Animation이란 말.)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-3.png"/>
</p>

---

## Unity-Chan에 Animation을 추가해 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-4.png"/>
</p>

* Unity Chang에 Animation을 붙이고 싶은데, 두 가지 방법이 있다.
  * Animation, Animator - Animation은 예전에 사용되던 컨트롤러이고 Animator를 쓴다고 생각하면된다.
* PlayerAnimController를 만들어 Animator Controller로 붙여보자.
  * Art\PlayerAnimController로 만듦

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-6.png"/>
</p>

* Animatior Controller에 Wait, Run을 넣어보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-5.png"/>
</p>

* Unity-Chan object에 PlayerAnimatorController를 적용 후 실행하면 Wait상대로 있음을 확인할 수 있다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-7.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-8.png"/>
</p>
