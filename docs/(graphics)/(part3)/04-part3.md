---
layout: default
title: "04. Quaternions"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이론

* 3차원 회전을 숫자 4개로 표현하는 방법
* `q = xi + yj + zk + w = (u, w)` -> `u = (x, y, z)`
    * 왜 4개로 표현하나? -> 짐벌락을 피하기 위해

* Quaternion의 예를 들어보자.
* 벡터 v를 회전축 x에 대해서 a만큼 회전시키려한다.
    * 1. 쿼터니언 `p = (v, 0)`를 정의한다.
    * 2. 회전 쿼터니언 `q = (sin(a/2) * x, cos(a/2))`를 정의한다.
    * 3. 켤레 쿼터니언 `q* = (sin(a/2) * -x, cos(a/2))`를 정의한다.
    * 4. 쿼터니언의 회전 `p' = q * p * (q*)`를 알수있다.
* 혹은 회전 쿼터니언(`q`)으로 부터 회전 행렬 쿼터니언을 구해 회전시킬수 있다.

```
// i, j, k는 허수이다.

i^2 = j^2 = k^2 = ijk = -1
ij = k = -ji
jk = i = -kj
ki = j = -ik
// 이 부분은 허수의 특징이니 증명하고 싶으면 하면 좋지만 ... 
// 그냥 받아들이는게 마음 편하다
```

```
ij = k = -ji
jk = i = -kj
ki = j = -ik

// 를 조금 설명하자면 ...

1 * i = i // 가 된다.
// 마치 Real에 i를 곱하면 90도 회전한 효과이다.
        y(image)
          |
          |
          |
          |
----------------------- x(Real)
          |
          |
          |

// 조금 더 확장해 보자.
// 2 + i에 i를 곱하면 -1 + 2i가 된다. 역시 90도 회전한 효과
// 복소수를 곱함으로 회전의 효과를 누릴 수 있다. 이걸 활용한다
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-4-1.png"/>
</p>

* 위 그림을 설명하자면 `ij = k` -> i를 j축으로 회전시키면 k가 나온다.
* `ji = -k` -> j를 i축으로 회전시키면 -k가 나온다.

---

