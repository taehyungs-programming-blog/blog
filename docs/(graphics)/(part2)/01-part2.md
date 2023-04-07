---
layout: default
title: "01. (기초) Vector, Point"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Vector와 Point 비교

* Vector와 Point가 모두 두 숫자의 집합이기에 헷갈린다.

* **Vector**
    * 방향과 크기를 표현
* **Point**
    * 위치만 표현

* 연산
    * `Point +/- Point = Vector`
    * `Point +/- Vector = Point`
    * 이렇게 보면 이해가 잘 되지 않고 예시로 보자

### `Point +/- Vector = Point` 예시

* 직선의 표현
    * Point(`P(0)`)에서 Vector(`ad`)를 더한다면
    * 방향과 크기가 반영된 `P(a)` Point가 된다.

* 선분(Line Segment) - 직선의 임의의 점
    * 직선의 시작점(`P(0)`) 끝점(`Q`)라면
    * 직선의 임의의 점(선분)은 `P(a)`라 표현가능하며
    * `P(a) = P(0) + a(Q - P(0)) = P(0) + ad = (1 - a)P(0) + aQ`
    * 간단히 말해 `a`가 0일때 Q가 0이되며 시작점 `P(0)`가 되며
    * `a`가 1일때 `P(0)`가 0이되며 끝점 `Q`가 된다.
    * 이걸 **Affine Combination**, **Affine Sum** 이라 부른다.

### `Point +/- Point = Vector` 예시

```
P
|\
| \
|  \ v
|   \
|    \
------
R      Q
   u
```

* Point : P, R, Q
* Vector : u, v

* 우린 R에서 P로 가는 Vector를 `u + v`라 정의할수 있음을 안다
    * `u = Q - R`, `v = P - Q`이기에
    * `u + v = P - R`로 역으로 증명이 가능.

---

## 추가) 행렬 벡터

```
u = (1, 2, 3) 이라면

u = [ 1 ]
    [ 2 ]
    [ 3 ]   연산을 위해 이렇게 세로로 표기하기도 한다.
            (보통 Transpos됐다는 의미로 t를 붙임)
```

### 동차좌표(Homogeneous Corrdinates)

```
u = (1, 2, 3) 라면 얘는 Vector인가 Point인가?

u = [ 1 ]
    [ 2 ]
    [ 3 ]
    [ 0 ] // 얘는 Vector입니다.

u = [ 1 ]
    [ 2 ]
    [ 3 ]
    [ 1 ] // 얘는 Point입니다.

이렇게 표기한다.
```