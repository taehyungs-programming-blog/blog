---
layout: default
title: "13. Scale, Rotation, Translation Matrix"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* Scale(사이즈), Rotation(회전), Translation(이동)에 대해 이해하고자 한다.

## Translation Matrix

* [x, y, z]에 있는 물체를 [x+1, y+2, z+3]이동하고자 한다면?
* [x, y, z] + [1, 2, 3] 너무 쉬운데??

* 인생이 이렇게 쉬우면 좋겠지만 … 우리는 효율성을 따져야한다.
* Rotation, Scale등을 함께 담은 Matrix(SRT(Scale, Rotation, Translation) Matrix)로 연산되기에 Translation또한 Matrix로 연산가능하게 해줘야 한다.

* 그렇담 어떻게 Translation을 Matrix로 표현이 가능할까?

```
// 우선 Vector를 아래와 같이 수정한다.
// (내가 Translation 하고자하는 물체의 좌표마지막에 1을 추가)
[x, y, z, 1]

// 그리고 아래와 같은 Matrix를 하나 만든다.
m11 m12 m13 m14
m21 m22 m23 m24
m31 m32 m33 m34
m41 m42 m43 m44
```

* 이제 Vector * Matrix를 해보면

```
X = x*m11 + y*m21 +z*m31 + m41
Y = x*m12 + y*m22 +z*m32 + m42
Z = x*m13 + y*m23 +z*m33 + m43
W = x*m14 + y*m24 +z*m34 + m44
```

* 여기까지하면 Matrix에 어떤값을 넣어야 할지 감이 온다.
* 내가 Translation 하고자 하는 좌표가 (a, b ,c)라면

```
1 0 0 0 
0 1 0 0
0 0 1 0
a b c 1
```

* 이게 Translation Matrix이다. (쉽군)

---

## Scale Matrix

* **(a배, b배 ,c배)**만큼 Scale하고 싶다면??

```
X = x*m11 + y*m21 +z*m31 + m41
Y = x*m12 + y*m22 +z*m32 + m42
Z = x*m13 + y*m23 +z*m33 + m43
W = x*m14 + y*m24 +z*m34 + m44
```

```
a 0 0 0 
0 b 0 0
0 0 c 0
0 0 0 1
```

* 이게 Scale Matrix이다. (더 쉽군)

---

## Rotation Matrix

* 일단 x, y, z축이 모두 회전하는것을 가정하고 설명하면 설명이 너무 어려우니
z축 회전(빙글빙글 돌기만한다)을 기준으로 설명하겠다.

* z축은 고정되어있기에 x, y만 고려하면되며 아래 그림의 A에서 B로 이동하는 것을 구한다고 생각하면 되겠다.

```
(y)
 |
 |      B(x1, y1)
 |
 |           A(x, y) 
 |
 |--------------- (x)
```

* 일단 원점에서 A, B의 거리는 z축이 고정되어 있기에 같을것이기에 d라고 정의한다.
* A점의 각도를 a라 할때 A점의 좌표를 표현해보자면 `(d*cos(a), d*sin(a))` 이라 표현이 가능하고
* B점의 각도를 a+b라 할때 B점의 좌표를 표현해보자면 (d*cos(a+b), d*sin(a+b)) 이라 표현이 가능하고
    * (참고로 b각도는 A, B점 사이의 각을 말함)

```
// (d*cos(a+b), d*sin(a+b))는 아래와 같이 표현가능

X = d*cos(a)*cos(b) - d*sin(a)*sin(b)
Y = d*sin(a)*cos(b) - d*cos(a)*sin(b)

// 초기 좌표 (x, y) = (d*cos(a), d*sin(a)) 이기에

X = x*cos(b) - y*sin(b)
Y = y*cos(b) - x*sin(b)

// 라고 식을 간단화 할 수 있다.
```

* 이제 이걸 Matrix로 표현만 하면 된다.

```
X = x*m11 + y*m21 +z*m31 + m41
Y = x*m12 + y*m22 +z*m32 + m42
Z = x*m13 + y*m23 +z*m33 + m43
W = x*m14 + y*m24 +z*m34 + m44
```

* 이기에

```
X = x*cos(b) - y*sin(b)
Y = y*cos(b) - x*sin(b)
```

```
// z축 회전
cos(b) -sin(b) 0 0
sin(b) cos(b)  0 0
0      0       1 0
0      0       0 1
```

```
// x축 회전
1 0      0       0
0 cos(b) -sin(b) 0
0 sin(b) cos(b)  0
0 0      0       1
```

```
// y축 회전
cos(b)  0 sin(b) 0
0       1 0      0
-sin(b) 0 cos(b) 0
0       0 0      1
```

<br>

---

## 핵심은 뭐냐?

* 위 식들이야 공식처럼 받아들일 수 있는 부분이다.
* 핵심은 Scale, Roation, Translation을 진행시 **Scale -> Rotation -> Translation** 순으로 해야한다는 점이다.
* 생각해보면 당연한게 Translation 후 Scale시 Translation된만큼 Scale되버린다. 등등 그냥 **SRT**라고 받아들이는게 마음편할수 있다.