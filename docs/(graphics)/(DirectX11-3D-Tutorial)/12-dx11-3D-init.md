---
layout: default
title: "12. (Math) Point, Intersection, Raycasting"
parent: "(DirectX11 3D Tutorial)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 복습) OBB를 위한 좌표계 변환

* 하나만 복습하고 들어가자
    * A좌표계에서 B좌표계로 변환하려한다 가정하자

```
M = 
ux uy uz 0
vx vy vz 0
wx wy wz 0 (-> B좌표계 기준, u, v, w 성분)
Qx Qy Qz 1 (-> B좌표계 기준, A의 좌표)
```

* 좀 더 설명하자면 ...
    * `ux uy uz` - B좌표계의 x축이 `ux uy uz`를 가리킨다
    * `vx vy vz` - B좌표계의 y축이 `vx vy vz`를 가리킨다
    * `wx wy wz` - B좌표계의 z축이 `wx wy wz`를 가리킨다
    * `Qx Qy Qz` - B좌표계의 원점이 A좌표계에 `Qx Qy Qz`에 위치한다

---

* 참고로 아래 모든것을 구현하진 않는다.
* DirectX의 SimpleMath에 많은것이 구현되어 있고 우린 이론적으로 어떻게 동작하나를 배워본다.

## Point

* Point(점)이 특정 도형에 포함이 되어있는지 확인하는 절차이다.

### Sphere

* Sphere에 Point가 포함되어있는지
    * 아주 간단하다. 원점에서 Point까지 거리를 구한 후 반지름 보다 작다면 Point내에 포함된 것이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-12-1.jpg"/>
</p>

---

### AABB

* 이것도 아주 쉽다. x, y, z의 min, max를 구해 point와 비교만 하면된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-12-2.webp"/>
</p>

---

### OBB

* 방식이 여러개 인데, 방식중 하나는 OBB좌표계로 Point를 변환하여 AABB방식으로 찾는게 있고
* 두 번째 방식은 OBB도형의 원점에서 Test Point로 Dir Vector를 만들고, 그 Vector를 각 축에 내적하여 내적된 거리가 Max보다 작은지 체크한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-12-3.webp"/>
</p>

```cpp
Vec3 dir = point - obb.position;

vector<Vec3> axis;
axis.push_back(obb.orientation.Right());
axis.push_back(obb.orientation.Up());
axis.push_back(obb.orientation.Backward());

vector<float> size;
size.push_back(obb.size.x);
size.push_back(obb.size.y);
size.push_back(obb.size.z);

for(int32 i = 0; i < 3; i++)
{
    float distance = dir.Dot(axis[i]);

    if(distance > size[i])
        return false;
    if(distance < -size[i])
        return false;
}

return true;
```

---

### Plane

* Plane의 Point의 위의 Noraml Vector와 Point를 내적하면 거리가 나오고
* 그 거리가 Plane의 거리와 Point의 거리가 일치하면 Plane위에 위치하게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-12-4.webp"/>
</p>

```cpp
float dot = point.Dot(plane.normal);

return cmp(dot - plane.distance, 0.0f);
```

---

### line

* 아주쉽다. ab vector를 만들고 ac가 ab의 스칼라배이면 line위에 있다.
* 그렇기에 얼마나 가까이 있는지 까지 체크해 보자.

```
(c) <- point
 | \
 |  \
 |   \
 |    \
(d)   (a) ------------- (b)
```

```cpp
Vec3 IVec = line.end - line.start;      // ab vector

// ab vector와 ac vector의 내적 후 ab 벡터의 내적으로 나눈다
    // ab를 ab로 내적시 제곱한 효과
// (|ac|*|ab|*cos) / (|ab|*|ab|) -> 이것과 식이 동일하다
// (|ac|*cos) / (|ab|) -> cos각도와 |ac|, |ab|에 따라 0~1의 값이 나오게된다.
// 여기서 부턴 잘 생각해보면 답이 나옴
float t = (point - line.start).Dot(IVec) / IVec.Dot(IVec);
t = fmaxf(t, 0.0f);
t = fminf(t, 1.0f);
return line.start + IVec * t;
```

---

### ray

* 방향벡터 ray가 있다면 point p에서 ray로 수직인 점을 구해보려한다.

```
       (p)
        |
        |
        |
        |
        |
(o) ----------------------------> (r)
        ?
```

```cpp
// op 벡터를 만들고 ray와 내적을 한다
float t = (point - ray.origin).Dot(ray.direction);
t = fmaxf(t, 0.0f);

// origin에서 direction에 t배만큼 하면 된다.
return Point3D(ray.origin + ray.direction * t);
```

---

## Intersection

---

## Raycasting

---

## Point in Triangle

