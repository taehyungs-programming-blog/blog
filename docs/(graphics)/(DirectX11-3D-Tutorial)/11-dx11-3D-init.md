---
layout: default
title: "11. Collider + Picking"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎](https://github.com/Arthur880708/DirectX11-3d-tutorials/tree/10)

## Picking 사전에 알아야 할 사항

* 800x600 화면이 있다고 가정하면 이 화면을 -1~1 x -1~1 범위로 변경해야한다.
* 추가로 단순 x,y좌표만 알아선 안되고 깊이값 z좌표까지 알아야한다.
    * 왜냐면 앞쪽에 있는 물체를 선택됐다고 판별해야하기 때문.
* 이런 작업을 처리하기 위해서 ViewPort Class를 별도로 만들어준다

```cpp
Vec3 Viewport::Project(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P)
{
    // world 좌표계에서 projection된 좌표계로
	Matrix wvp = W * V * P;
	Vec3 p = Vec3::Transform(pos, wvp);

    // 아래식은 projection to pos로 받아들이자
	p.x = (p.x + 1.0f) * (_vp.Width / 2) + _vp.TopLeftX;
	p.y = (-p.y + 1.0f) * (_vp.Height / 2) + _vp.TopLeftY;
	p.z = p.z * (_vp.MaxDepth - _vp.MinDepth) + _vp.MinDepth;

	return p;
}

Vec3 Viewport::Unproject(const Vec3& pos, const Matrix& W, const Matrix& V, const Matrix& P)
{
    // projection 좌표계에서 world된 좌표계로
	Vec3 p = pos;

	p.x = 2.f * (p.x - _vp.TopLeftX) / _vp.Width - 1.f;
	p.y = -2.f * (p.y - _vp.TopLeftY) / _vp.Height + 1.f;
	p.z = ((p.z - _vp.MinDepth) / (_vp.MaxDepth - _vp.MinDepth));

	Matrix wvp = W * V * P;
	Matrix wvpInv = wvp.Invert();

	p = Vec3::Transform(p, wvpInv);
	return p;
}
```

* Tip) Viewport 변환행렬

```
M = 
W/2      0       0         0
0        -W/2    0         0
0        0       MaxD-MinD 0
W/2+Left W/2+Top MinD      1
```

---