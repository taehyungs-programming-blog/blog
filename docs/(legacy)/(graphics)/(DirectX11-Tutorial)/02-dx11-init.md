---
layout: default
title: "02. (Math) Matrix"
parent: "(DirectX11 2D Tutorial)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## SRT (Scale - Rotate - Translation)

### Translation

* x, y, z좌표를 a, b, c만큼 **Matrix를 곱하여** Translation하려면 어떻게 해야할까?
* 그냥 정답부터 보여주면

```
* 목표는
X = x + a
Y = y + b
Z = z + c


* 우선 기존 좌표계에 마지막 항에 1을 추가

v = [x y z 1]

M = 
1 0 0 0 
0 1 0 0
0 0 1 0
a b c 1

v * M = translation된 좌표
```

---

### Scale

```
* 목표는
X = ax
Y = by
Z = cz


* 우선 기존 좌표계에 마지막 항에 1을 추가

v = [x y z 1]

M = 
a 0 0 0 
0 b 0 0
0 0 c 0
0 0 0 1
```

---

### Rotatoin

```
          (X,Y)   (x,y)
   |       /       /
   |      /       /
   |     /      /
   |(b) / (a) /
   |   /    /
(0, 0)
```

* a각에서 b각으로 회전하려한다
* 우선, (X, Y)를 구해보면
    * X = R * cos(a + b) - [ R = 직선거리 ]
    * Y = R * sin(a + b)
* 다음으로 (x, y)를 구해보면
    * x = R * cos(a)
    * y = R * sin(a)

```
X = R * cos(a + b)
= R * cos(a) * cos(b) - R * sin(a) * sin(b)
= x * cos(b) - y * sin(b)

Y = R * sin(a + b)
= R * sin(a) * cos(b) + R * cos(a) * sin(b)
= y * cos(b) + x * sin(b)


* Matrix로 바꿔보자면

* 목표(단, 지금은 x, y축만 생각한다)
X = x * cos(b) - y * sin(b)
Y = x * sin(b) + y * cos(b)
Z = z

M = 
cos(b) -sin(b) 0 0
sin(b) cos(b)  0 0
0      0       1 0
0      0       0 1
```

* 현재는 특정 축(x,y)만 고려하고 다른 축까지 고려하진 않음

---

### 주의할 점?

* SRT순서대로 Matrix를 곱해야한다.
    * 가령 Translation이 되고(다른위치로 가고) Rotaiton이 되어버리면, 제자리에서 돌릴꺼(자전)를 크게 돌게(공전) 된다.

---

## 이제 좌표계를 변환해 보자

```
                 \    /
|                 \  /   
|                  \/
|        M(x,y)     b
|
|
|
----------------------
a
```

* aM 벡터를 b좌표계로 변환하려한다.
* `aM = x*u + y*v`라 표현가능하다 (u = x축 단위 벡터, v = y축 단위 벡터)
* b좌표계의 단위 벡터를 `U`, `V`라 가정하자
    * 따라서, `bM = X*U + Y*U`라 표현이 가능해 진다.
    * `u = ux*U + uy*V`로 b좌표계로 변환이 가능하고
    * `y = vx*U + vy*V`로 b좌표계로 변환이 가능하다

```
aM = xu + yv
= x*(ux*U + uy*V) + y*(vx*U + vy*V)
= (x*ux + y*vx)*U + (x*uy + y*uy)*V

* 일단 aM을 b좌표계의 단위벡터 U, V로 표현이 가능함을 기억하자
```

```
bM = aM - ab = aM + ba
= (x*ux + y*vx)*U + (x*uy + y*uy)*V + (Qx*U + Qy*V)
= (x*ux + y*vx + Qx)*U + (x*uy + y*uy + Qy)*V

* 참고로 Qx, Qy는 b좌표계를 기준으로 한 a의 위치이다.
```

* ??? 이걸 왜 하고있지?
* 우선 알아야 할게 `x,y`는 a좌표계 기준 현재 위치이다.
* 다음으로 `ux, uy`는 b좌표계 기준 현재 위치이다.
* `Qx, Qy`는 b좌표계를 기준으로 한 a의 위치이다.
* b좌표계의 표현(`(x*ux + y*vx + Qx)*U + (x*uy + y*uy + Qy)*V`)이 `ux, uy`, `vx, vy` ,`Qx, Qy`만 알고있으면 가능하다는걸 알수 있다.

```
v = [x y z 1]

M =
ux uy uz 0
vx vy vz 0
wx wy wz 0
Qx Qy Qz 1
```

* 그럼 `ux, uy`, `vx, vy` ,`Qx, Qy`만 구하면 되겠구만?
    * 좀 더 설명하자면 `ux, uy`, `vx, vy`는 b좌표계 기준 성분이라 볼 수 있고
    * b좌표계 기준 `Qx, Qy`는 위치라 볼수있다
    * (TIPS) 따라서 `[x y z 0]`에 마지막 항을 0으로 넣을 경우 위치성분은 빼고 마치 단위 벡터처럼 사용이 가능하다
* 와 ... 너무 어려운데 ...
* **중요한 거만 기억하자** - 핵심은 **b좌표계 기준 성분**과, **b좌표계 기준 위치**만 알수 있으면 언제든 좌표계 변환이 가능하다는 것! -> 이것으로 좌표계 변환을 구한다.

---

## Local to World

* 구하고자 하는 핵심은 World 좌표계 기준 x, y, z 성분과 위치를 구하면 Matrix를 만들 수 있다.
* 사실 Local to World는 별겨없다 SRT만 해주면 됨.

---

## World to View

* 여기서 View는 카메라를 의미한다.
    * 말인즉슨 카메라가 있는 곳이 원점이 된다!
* 어떻게 구할까?
    * 이렇게 생각하면 편하다. 카메라가 오른쪽으로 이동시 마치 모든 물체는 왼쪽으로 이동한 효과를 보이면 된다.
    * 이렇게 볼수 있지 않을까? 카메라의 역행렬? 그런데 World좌표는 모두 SRT로 표현이 가능하다고 위에서 했고, 카메라는 Scale은 의미없기에 S를 무시하면
    * View = W(cam)^-1 = (RT)^-1 = T^-1 * R^-1 = T^-1 * R^T (Rotation은 직교행렬이라 Transpose가능)

```
* T^-1
1    0   0  0
0    1   0  0
0    0   1  0
-Cx -Cy -Cz 1

* R^T
Right(x) Up(x) Look(x) 0
Right(y) Up(y) Look(y) 0
Right(z) Up(z) Look(z) 0
0        0     0       1

* T^-1 * R^T
Right(x)  Up(x)  Look(x)  0
Right(y)  Up(y)  Look(y)  0
Right(z)  Up(z)  Look(z)  0
-Cx*Right -Cy*Up -Cz*Look 1
```

---

## View to Projection

* 원근감을 어떻게 반영할 것인가 + 물체뒤에 있는애는 어떻게 안그릴것인가 핵심이다.

```
y
|
|
|
|             o
|
|   o
-------------------------------- z
   (c)    (n)             (f)
```

* n - 화면에 그려지는 최소 거리
* f - 화면에 그려지는 최대 거리
* c - 실제 화면은 여기
    * c를 1이라 가정한다면 비례식에 의해서
    * `X = x / z`, `Y = y / z`가 된다
    * 만약 c가 1이 아니라면 촬영각을 a라 하면 `tan(a/2)`를 반영해 주면 된다.
* + 화면 비율까지 반영 `X = x / (r*z)`, `Y = y / (r*z)` / `r = w / h`
* `X = x / (r*z*tan(a/2))`, `Y = y / (z*tan(a/2))`

* 이제 중요한건 이걸 Matrix로 만들어야한다.

```
M = 
1 / (r*z*tan(a/2)) 0                0 0
0                  1 / (z*tan(a/2)) 0 0
0                  0                1 1
0                  0                0 0
```

---

## Simple Math

* [SimpleMath](https://github.com/Microsoft/DirectXTK/wiki/SimpleMath) - MS에서 제공하는 라이브러리(소스코드로제공됨)이다.

```cpp
// 요런식으로 활용할 예정
using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
```

```cpp
void Game::Update()
{
	// Scale Rotation Translation

	_localPosition.x += 0.001f;

	Matrix matScale = Matrix::CreateScale(_localScale / 3);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z);
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	Matrix matWorld = matScale * matRotation * matTranslation; // SRT
	_transformData.matWorld = matWorld;

	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(subResource));
	
	_deviceContext->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData));
	_deviceContext->Unmap(_constantBuffer.Get(), 0);
}
```