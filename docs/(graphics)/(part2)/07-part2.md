---
layout: default
title: "07. Culling"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Git Clone 🌎](https://github.com/Arthur880708/DirectX11_Part2_Rasterization/tree/main/7_backfaceculling)

* 간단히 말하면 화면에 보여지지 않을, Object의 뒷 면이나 가려질 부분은 그려주지 않는 것이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/d11/d11-7-1.jpeg"/>
</p>

* 어떻게 하냐?
    * 이전에 사용한 Edge Function을 이용한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/d11/d11-3-2.jpg"/>
</p>

