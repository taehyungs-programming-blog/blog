---
layout: default
title: "14. World <-> View 변환"
parent: "(DirectX12 Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 3D Game Engine에서 사용되는 좌표계의 종료

* **Local** : 각 3D 오브젝트 하나하나의 좌표계
    * Ex) 사람모양을 한 오브젝트가 있다면 발끝(원점)을 기준으로 손은 (x, y, z) 머리는 … 이런식
* **World** : 전체 월드에 오브젝트를 두고 그리는 좌표계
    * 각 오브젝트에 월드행렬을 곱하면 자신의 위치가 나타난다
* **View** : 카메라의 위치를 기준으로 찍히는 화면의 좌표계
* **Projection(투영)** : 카메라에 의해 나타날 부분/나타나지 않을 부분, 작게/크게 표현할 부분을 담당하는 좌표계
* **Screen** : 실제로 모니터(해상도)에 그려질 부분의 좌표계
* 3D Game Engine은 보통 위의 5개의 좌표계를 쓰며 서로서로간 좌표계 변환이 필요하다
    * 사실 이게 설명으로 보면 이해가 어려움. 오히려 수식이 쉽다

* 여기서 필요한 정보는 좌표계 변환(원점 변환)을 어떻게 하는지에 대한 수식이 필요하다

---

## 좌표계 변환

* 좌표계 변환을 어떻게 구해야할까?

```
(y)
 | 
 |     M(x, y)
 |
 |
 |
 |------------------- (x)
 A
```

* 원점을 A라 가정하고 x, y축의 단위 백터를 u1, v1이라 가정하자
* AM의 벡터를 표현 하는 방법은 `AM = x*u1 + y*v1` 이라 표현할 수 있다.

* 여기서 문제는 원점이 변경되었을 때 (가령 카메라의 좌표, 각도가 변경되었을 경우)
* 변경된 원점 B에 대해서 상대적으로 M의 좌표를 어떻게 표현할꺼냐에 대한 문제이다.
    * (우리가 갖고있는 값은 M의 좌표 및 새로운 원점의 각도 정도이다.)

* 좀 정리하자면 우린 `AM = x*u1 + y*v1`을 알고 있고
* 원점이 변경된 `BM`을 구하고 싶은데 `x, u1, y, v1`을 통해 표현해야한다.

* 아래와 같이 기존원점 A에서 B로 원점임 변경됐다고 생각해 보자.

```
(y)
 | 
 |     M(x, y)\   /
 |             \ /
 |              B
 |
 |------------------- (x)
 A
```

* B가 새로운 원점좌표이고 B를 기준으로 M을 표현(BM)하고자 한다면??
* `BM = x2*u2 + y2*v2` 여기서 (x2, y2)는 B에서 M을 봤을때 상대적 위치 u2, v2는 B원점좌표의 단위 벡터이다.

```
// AM을 단위벡터 u1, v1로 아래와 같이 표현가능하고
AM = x*u1 + y*v1
// u1, v1도 결국은 u2, v2로 표현이 가능하기에

// ux, uy, vx, vy는 AM기준 단위벡터에서 BM기준 단위 벡터로 
// 표현시 필요한 성분의 단위벡터
u1 = ux*u2 + uy*v2  
// u1을 u2, v2 성분의 합으로 표현하고 싶은데
// ux, uy란 값을 곱해주면 가능하다고 가정한다
v1 = vx*u2 + vy*v2
```

```
// 일단 하려는게 u1, v1으로 표현된 AM을 u2, v2로 표현을 변경하려는게 목적이다.
AM = x*u1 + y*v1
AM = x*(ux*u2 + uy*v2) + y*(vx*u2 + vy*v2)
AM = u2*(x*ux+y*vx) + v2*(x*uy+y*vy)
// 여기까지하면 AM을 u2, v2로 표현이 가능해진다.
```

```
// 벡터상으로 볼때
AM = AB + BM  // 이거는 벡터 공식

// 우리가 알고 싶은 것은 BM이기에
BM = BA + AM
BM = AM + BA

// 라고 표현이 가능해진다.
// AM = u2*(x*ux+y*vx) + v2*(x*uy+y*vy) 라고 위에서 증명했기에
BM = u2*(x*ux+y*vx) + v2*(x*uy+y*vy) + BA
// 라고 도출이 된다.
```

* BA또한 B를 기준으로 A를 바라보는 벡터이기에

```
// 여기서 ux, uy, vx, vy, qx, qy는 알수없음.

BA = qx*u2 + qy*v2
// 라고 표현해보자면
// 우선 BM = u2*x2 + v2*y2라고 정의한걸 잊지말자

BM = u2*(x*ux+y*vx) + v2*(x*uy+y*vy) + qx*u2 + qy*v2
// 좀 정리하자면
BM = u2*(x*ux + y*vx + qx) + v2*(x*uy + y*vy + qy)

// x2 = x*ux + y*vx + qx
// y2 = x*uy + y*vy + qy

// 라고 도출되며, ux, uy, vx, yv, qx, qy만 알면
// 기존 좌표 (x, y)에서 새로운 원점의 좌표 (x2, y2)를 Matrix를 곱하는 형태로 구할 수 있다.
```

* 3차원으로 정리하면 아래와 같은 Matrix가 도출된다.

* 즉, 내 좌표계 `[x, y, z, 1]`에 Matrix를 곱할경우 변경된 좌표계에서 내 위치(x, y, z)를 구할 수 있다.
    * 그럼 이제 Matrix구하는 방법만 알면되겠군?? 
    * (맞다. 사실 Matrix구하는 방법만 알면되고 위의 설명은 이런식으로 좌표계 변환이 이루어진다고 알려주는 것이다.)

* 추가적으로 [x, y, z, 0] 마지막 항을 0으로 넣을시 qx, qy, qz Translation(위치) 정보가 모두 무시되기에 방향성만 표현하는 Vector가 될수 있음

* **결론은 ...** 설명은 어렵게 했는데 좌표계 **변환 매트릭스(Translation Matrix)**를 곱하면 변경된 좌표계에서 해당 오브젝트의 좌표가 나오게 할 수 있다는 말!.
    * 좌표계 변환시 `(x, y, z, 1) * [변환 Matrix]`만으로 좌표계 변환을 구할 수 있다.
    * 말인 즉슨 **변환 Matrix**만 알면 된다!!
* 그리고 그 **Trasnlation Matrix**는 결국 좌표계를 옮긴 좌표의 단위 벡터의 Matrix로 구성이 된다.

```
[x, y, z, 1] * [ Ux Uy Uz 0 ]
               [ Vx Vy Vz 0 ]
               [ Wx Wy Wz 0 ]
               [ Qx Qy Qz 1 ]

// Ux, y, z / Vx, y, z / Wx, y, z는 새 좌표계의 단위벡터
// U - x축 단위벡터
// V - y축 단위벡터
// W - z축 단위벡터
// Q - 위치벡터
```

---

## Local <-> World

아래와 간은 좌표계가 있다고 가정하자

```
(up)
 |     (look)
 |     /
 |    /
 |   /         O(x, y, z)
 |  /
 | /
 |------------------------- (right)
```

* Local 좌표계에서 Rotation만 up축의 a도 돌리고 World 좌표계에서 (x, y, z)만큼 이동한 물체의 World 좌표계 변환을 구해보자

```
right.x right.y right.z 0
up.x    up.y    up.z    0
look.x  look.y  look.z  0
pos.x   pos.y   pos.z   1
```

* `right.x, y, z / up.x, y, z / look.x, y, z` : 각 방향의 단위 벡터이다.
    * **물론 모두 현재는 모르는 값이다. 이걸 구하는게 목표!**
* `pos.x, y, z` : 오브젝트가 이동할 좌표이다.

* 위 Matrix에서 우리가 현재알고 있는 부분은 부분은 `pos(x, y, z)`뿐이고 right, up, look은 어떻게 구해야할까?

* 앞에서 배운 SRT(Scale, Rotation, Translation)가 Matrix의 right, up, look이게 된다.
* Scale -> Rotation -> Translation을 하게 되면 자연스럽게 World Matrix가 나오게 된다.

* `(x, y, z, 1)*[Scale Matrix]*[Rotation Matrix]*[Translation Matrix]` 이렇게 하면됨
* 각 Matrix를 어떻게 구하는지 헷갈린다면 앞의 강의를 참조.

```
# Scale Matrix
a 0 0 0 
0 b 0 0
0 0 c 0
0 0 0 1
```

```
# Rotation Matrix
// z축 회전
cos(b) -sin(b) 0 0
sin(b) cos(b)  0 0
0      0       1 0
0      0       0 1

// x축 회전
1 0      0       0
0 cos(b) -sin(b) 0
0 sin(b) cos(b)  0
0 0      0       1

// y축 회전
cos(b)  0 sin(b) 0
0       1 0      0
-sin(b) 0 cos(b) 0
0       0 0      1
```

```
# Translation Matrix
1 0 0 0 
0 1 0 0
0 0 1 0
a b c 1
```

* 여기까지하면 Local에서 World로 변환이 완성된다.

---

## World <-> View

* View(카메라)역시 World행렬에 하나의 오브젝트로 있고, View가 찍고있는 영역을 View 좌표계가 된다.

```
# 아래가 모두 World 좌표계 안에있음

\              /
 \  O(물체)   /
  \          /
    (카메라)
```

* 위 그림처럼 보자면 카메라가 오른쪽으로 돌면 물체는 왼쪽으로 간다고 생각
* 왼쪽으로 돌면 물체는 오른쪽으로 간다고 생각 할 수 있다.

* 이게 View 좌표계를 구하는 힌트이다.

* 카메라의 World Matrix는 아래와 같고
    * **up** : 카메라의 z축
    * **look** : 카메라가 찍고있는 방향
    * **right** : 카메라가 찍고있는 방향(look)의 수직직교

```
(up)
 |     (look)
 |     /
 |    /
 |   /
 |  /
 | /
 |------------------------- (right)
```

```
right.x right.y right.z 0
up.x    up.y    up.z    0
look.x  look.y  look.z  0
x       y       z       1
```

* 사실상 반대로 동작해야하니 역행렬을 구하면 된다.
* SRT가 카메라의 World Matrix이고 카메라는 S(Scale)이 없기에 `RT`가 World Matrix가 된다.
* 결국 RT의 역행렬을 구하면 되고 `(RT)^-1 = T^-1*R^-1`
* right, up, look은 각 축이기에 **직교성이 보장되어 역행렬이 단순하다**

* 이렇게 생각하면 오히려 쉽다.
    * 카메라가 기준(원점)이 됐다고 생각하고, 카메라의 좌표계에 각 오브젝트를 위치시킨다면?
        * 예시로 카메라가 오른쪽으로 움직이면 찍히는 물체는 상대적으로 왼쪽으로 움직이게 된다.
    * 즉, World좌표계를 Local좌표계(카메라 기준)로 변환해야한다.

```
// T^-1
1  0  0  0 
0  1  0  0
0  0  1  0
-x -y -z 1

// 이렇게도 표현이 가능
1    0    0    0 
0    1    0    0
0    0    1    0
-c.x -c.y -c.z 1
```

* c(camera) vector이고 c.x는 c의 x축을 의미함 기존의 x와 완전동일

```
// R^-1
// 이게 가능한게 right, up, look은 orthogonal하기에 가능함
// 일반 Matrix의 Transpos는 그냥 뒤집는다고 되지는 않음(그냥 알고만 있자..)
right.x up.x look.x 0
right.y up.y look.y 0
right.z up.z look.z 0
0      0     0      1
```

```
// T^-1*R^-1
right.x  up.x  look.x  0
right.y  up.y  look.y  0
right.z  up.z  look.z  0
-c*right -c*up -c*look 1
```

* 이걸 어떻게 구현하지??? 걱정하지말자 MS가 미리준비했다.
* 여기까지하면 카메라 입장에서 찍히는 Object구현끝!

```cpp
// View 변환 행렬을 리턴해준다.
XMMATRIX XM_CALLCONV  noexcept XMMatrixLookAtLH(
  FXMVECTOR EyePosition,    // 오브젝트의 현 위치
  FXMVECTOR FocusPosition,  // 카메라가 주시하고 있는 점
  FXMVECTOR UpDirection     // 대략적 up벡터 보통 (0, 1, 0)
);
```

```
 (up)
  |    
  |     
  |   
  |           
  |  
  | 
(Eye)-------------------------> (Focus)
```

* 간략하게 동작원리를 설명하자면
* EyePosition에서 FocusPosition으로 선을 그으면 Look vector가 생성된다.
* Up과 Look으로 평면을 만들고 그 평면의 수직선으 Right vector를 만든다.
* Look과 Right를 외적해 다시 실질적 Up vector를 생한다.

* 이러한 방법으로 View Matrix를 생성한다.