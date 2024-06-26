---
layout: default
title: "7-5. KeyFrame Animation"
parent: "Link Page"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌍](https://github.com/EasyCoding-7/unity_tutorials/tree/7.5)

* 루틴하게 움직이는 오브젝트는 어떻게 처리해야할까?(NPC 등)
* Window -> Animation(Ctrl + 6)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-5-1.png"/>
</p>

* Cube하나를 움직이게 해보자. Animation 창을 열어둔채로 Cube하나를 클릭한다.
* Add Property를 클릭해 Position, Rotation, Scale을 추가하자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-5-2.png"/>
</p>

* Key를 추가하면 상태를 변경할 수 있는데,
  * 우클릭 후 Add Key 혹은
  * 특정 시간에 위치시킨 후 Object에 Add Key를 해도된다(그냥 해보는게 편함)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/csharp/unity/unity-7-5-3.png"/>
</p>

* 더 쉽게는 녹화 버튼을 누른 후 object를 움직이면 자동으로 keyframe이 저장이 된다.

* (TIPS) Curves를 통해 Blend를 넣어줄 수 있음.