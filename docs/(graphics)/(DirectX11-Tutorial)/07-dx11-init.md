---
layout: default
title: "07. 인스턴싱"
parent: "(DirectX11 2D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Instancing

* 이런 처리는 어떻게 할까?
* 아래와 같이 네모가 여러개 있다고 가정하자, 이 경우 모든 네모는 렌더링 파이프라인을 통과할까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-7-1.png"/>
</p>

* 아무런 옵션을 넣지 않았다면 그렇다! 모든 네모는 렌더링 파이프라인을 거친다.
    * 이게 무슨 비효율인가!!
* 아래 그림의 Batch가 렌더링 파이프라인을 거친지를 의미한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-7-2.png"/>
</p>

* 그럼 옵션에서 Dynamic batching을 켜보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-7-3.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-7-4.png"/>
</p>

* 확실히 Batch가 줄었다.
* 이번 장에선 이 인스턴싱을 공부해볼 예정이다.

---

## Instancing을 적용해 보자

