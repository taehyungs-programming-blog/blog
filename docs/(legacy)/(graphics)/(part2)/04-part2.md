---
layout: default
title: "04. 원 그리기"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Git Clone 🌎](https://github.com/Arthur880708/DirectX11_Part2_Rasterization/tree/main/2_Circle)

## 이론

* 삼각형을 여러개 그려서 원을 그려보자.

```
* 들어가기 전 알고가자! *

|       r (cos(a), sin(a))
|     /
|    /
|   /
|  /
| /
|/ a
----------------------

- 일반적으로 x, y축의 특정 지점을 다음과 같이 표현가능하다
    - x = cos(a)
    - y = sin(a)
```

---

## 구현

* 간단하기에 소스코드 참고