---
layout: default
title: "02. 레스터화(Rasterize)에 대해"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 레스터화(Rasterize)

* 일반적으로 우리가 상상할 수 있는 랜더링 기법은 **역방향 광추적**이다.
* **역방향 광추적**이란 그려질 모든 픽셀에서 Ray를 쏘아 어떤 픽셀의 색을 결정하게 된다.
    * 그런데 이게 효율적일까?
    * 4k모니터에서 8k모니터를 사용하게 된다면?
* 새로운 방법이 필요하다 -> **레스터화**
* **레스터화**란 내가 그릴 삼각형을 모니터의 방향으로 Ray를 쏘아 그려질지 말지를 결정
    * 삼각형 밖에 있을경우 색을 그냥 비워버리면 된다.

---

## (TIP) 무게중심 좌표계 유도

* T를 PQR로 표현해 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/d11/d11-2-1.jpg"/>
</p>

* PQR로 T를 표현이 가능하며

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/d11/d11-2-2.jpg"/>
</p>

* 삼각형의 부분 삼각형의 넓이를 표현이 가능해진다.

* 아직은 뭔 소린가 싶겠지만 이후에 이용이되니 기억만 해두자.