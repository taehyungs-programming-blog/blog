---
layout: default
title: "01. 플레이 해보기"
parent: "(Unreal Network)"
grand_parent: "(Unreal 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/Unreal_Example_Network/tree/1)
* 소스코드를 다운받고 기존 코드는 우선 받아들인다.
    * 이번강의의 핵심은 네트워크!

---

## 플레이해보기

* Net Mode를 **Play As Listen Server**로 설정

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/unreal_network_1_1.png"/>
</p>

* 실행후 `Shift + F1`을 눌러 화면 밖으로 이동

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/unreal_network_1_2.png"/>
</p>

* 버튼을 눌러 다른 클라이언트를 생성
* 서로 움직임이 동기화 됨을 알 수 있다.

---

## 어떻게 했을까?

* 사전사항이 몇 가지 필요하다
* Editor Preferences -> Allow late joining -> True

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/unreal_network_1_3.png"/>
</p>

* 아래의 버튼이 보이게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/network/unreal_network_1_2.png"/>
</p>

