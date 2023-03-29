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
    * 1. 방향 쿼터니언 `p = (v, 0)`를 정의한다.
    * 2. 회전 쿼터니언 `q = (sin(a/2) * x, cos(a/2))`를 정의한다.
    * 3. 켤레 쿼터니언 `q* = (sin(a/2) * -x, cos(a/2))`를 정의한다.
    * 4. 쿼터니언의 회전 `p' = q * p * (q*)`인데 이 증명은 생략 ... (대략 설명하면 Rotation Matrix의 값과 쿼터니언의 회전의 값이 같음)
* 혹은 회전 쿼터니언(`q`)으로 부터 회전 행렬 쿼터니언을 구해 회전시킬수 있다.
  * `q = (q1, q2, q3, q4)`라 할때
  * 회전 매트릭스는

```
R =
[ R11 R12 R13 ]
[ R21 R22 R23 ]
[ R31 R32 R33 ]
=
[1-2q2^2-2q3^2 2q1q2-2q3q4 2q1q3+2q2q4]
[2q1q2+2q3q4 1-2q1^2-2q3^2 2q2q3-2q1q4]
[2q1q3-2q2q4 2q2q3+2q1q4 1-2q1^2-2q2^2]
```

* 쿼터니언을 조금 더 자세하게 살펴보자.

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

* 쿼터니언은 4차원이기에 그림으로 설명이 어려우나 위 처럼 4개의 축을 그려놓고 설명하면,
* `ij = k` -> i를 j축으로 회전시키면 k가 나온다.
* `ji = -k` -> j를 i축으로 회전시키면 -k가 나온다.

* 결론?
  * 짐벌락을 피하기 위해 쿼터니언을 사용
  * 쿼터니언은 3개 축을 동시에 이동하기에 짐벌락을 피할 수 있다.
  * 또한 로테이션 매트릭스로 표현이 가능해 쉐이더 연산에 유리하다.

---

## 구현

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/4/4.quaternion)

```cpp
void ExampleApp::Update(float dt) {

    // ...

    // Quaternion Rotation
    // 생성자 중에서 Quaternion(Vector3&, float)를 사용
    Vector3 n = Vector3(m_quatX, m_quatY, m_quatZ);
    n.Normalize();
    Quaternion q(n * sin(m_quatTheta * 0.5f), cos(m_quatTheta * 0.5f));

    m_mainSphere.UpdateModelWorld(Matrix::CreateFromQuaternion(q) *
    								  Matrix::CreateTranslation(translation));


    m_mainSphere.m_basicVertexConstantData.view = viewRow.Transpose();
    m_mainSphere.m_basicVertexConstantData.projection = projRow.Transpose();
    m_mainSphere.m_basicPixelConstantData.eyeWorld = eyeWorld;
    m_mainSphere.UpdateConstantBuffers(m_device, m_context);
```