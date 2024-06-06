---
layout: default
title: "11. (Lyra) Shooter Core Plug-in"
parent: "(Example6 - Lyra 분석)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌟](https://github.com/Arthur880708/LyraClone/tree/2)

---

## 시작

* Lyra의 Shooter Core Plug-in을 분석해 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-1.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-3.png"/>
</p>

---

## ShooterCore Data Asset를 먼저 확인하자

* Data Asset 이란?
    * 새로운 게임을 정의한다 생각하자.
    * 모든 세팅을 Project Settings에서 할 수 없으니 Data Asset에 나눈다라고 생각하면 편하다
* 가장 베이스가 되는 Data Asset은 ShooterCore Data Asset을 검색해 보면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-4.png"/>
</p>

* 뭔가 게임과 관련된 세팅이 잔뜩이다.
* 그래서 이걸 어디서 쓰느냐? 그건 이제 확인해봐야 할 문제 ...

---

## 실제 예시로 살펴보자 (Weapon)

* ID_Pistol을 먼저 살펴보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/unreal_cpp_6/ucpp6-11-5.png"/>
</p>

> 여기서 부터는 직접 분석해 보는게 좋음.
