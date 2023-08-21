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

### Sphere to Sphere

* 아주 쉽다. 원점간 거리를 구하고 두 반지름 보다 작다면 Intersection!

---

### Sphere to AABB

* 위의 Point에서 구한 Closest Point를 찾아 반지름 보다 작다면 Intersection

---

### Sphere to OBB

* AABB와 동일함
* 위의 Point에서 구한 Closest Point를 찾아 반지름 보다 작다면 Intersection

---

### Sphere to plane

* 역시 동일함
* 위의 Point에서 구한 Closest Point를 찾아 반지름 보다 작다면 Intersection
* 이로서 Point에서 **Closest Point**를 구하는게 왜 중요한지 알수있을것이다.

---

### AABB to AABB

* x, y, z 세 축으로 min, max를 비교하여 겹치는비 확인

```cpp
Point3D aMin = AABB3D::GetMin(aabb1):
Point3D aMax = AABB3D::GetMax(aabb1):
Point3D bMin = AABB3D::GetMin(aabb2):
Point3D bMax = AABB3D::GetMax(aabb2):

return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
      (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
      (aMin.z <= bMax.z && aMax.z >= bMin.z);
```

---

### AABB to OBB

* SAT(Separating Axis Theorem)을 이용하는데, 간단히 말하면 AABB의 세 축, OBB의 세 축, AABB, OBB의 외적한 Matrix의 세 축을 모두 AABB to AABB처럼 분석하는것이다.
  * 총 15개의 축을 비교하면 된다.

```cpp
// AABB, OBB의 Min, Max가 필요하기에 Min, Max를 구해주는 함수를 파자
Interval3D GetInterval(const AABB3D& aabb, const Vec3& axis);
Interval3D GetInterval(const OBB3D& obb, const Vec3& axis);
```

```cpp
bool OverlapOnAxis = [](const AABB3D& aabb, const OBB& obb, Vec3& axis) {
  Interval3D a = GetInterval(aabb, axis);
  Interval3D b = GetInterval(obb, axis);
  return ((b.min <= a.max) && (a.min <= b.max));
}

Vec3 test[15] = {
  Vec3(1, 0, 0),          // AABB axis
  Vec3(0, 1, 0),  
  Vec3(0, 0, 1),
  obb.orientation.Right(),// OBB axis
  obb.orientation.Up(),
  obb.orientation.Backward(),
};

// 외적으로 남은축을 넣는다
for(int i = 0; i < 3; i++){
  test[6 + i * 3 + 0] = test[i].Cross(test[0]);
  test[6 + i * 3 + 1] = test[i].Cross(test[1]);
  test[6 + i * 3 + 2] = test[i].Cross(test[2]);
}

for(int i = 0; i < 15; i++) {
  if(OverlapOnAxis(aabb, obb, test[i]) == false)
    return false;
}

return true;
```

---

### AABB to plane

```cpp
float pLen = aabb.size.x * fabsf(plane.normal.x) +
              aabb.size.y * fabsf(plane.normal.y) +
              aabb.size.z * fabsf(plane.normal.z);

float dot = plane.normal.Dot(aabb.position);
float dist = dot - plane.distance;

return fabsf(dist) <= pLen;
```

---

### OBB to OBB

```cpp
Vec3 test[15] = {
  // AABB to OBB와 이 부분만 다르다
  obb1.orientation.Right(),// OBB1 axis
  obb1.orientation.Up(),
  obb1.orientation.Backward(),
  obb2.orientation.Right(),// OBB2 axis
  obb2.orientation.Up(),
  obb2.orientation.Backward(),
};
```

---

## Raycasting

### Raycast Sphere

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-12-1.png"/>
</p>

```cpp
// e - ray원점에서 sphere 중심까지 vector
Vec3 e = sphere.position - ray.origin;

float rSq = sphere.radius * sphere.radius;
float eSq = e.LengthSquared();

// a - 원점에서 중심까지 직선거리
float a = e.Dot(ray.direction);

// bSq - 원점에서 ray의 직선거리까지 거리
float bSq = eSq - (a * a);
float f = sqrt(rSq - bSq);

// No Collision
if(rSq - (eSq - (a * a)) < 0.0f)
  return false;

if(eSq < rSq)
{
  // ray가 안에서 시작하는 케이스
  distance = a + f;
  return true;
}

// Normal intersection
distance = a - f;
return true;
```

---

### Raycast Axis Aligned Bounding Box

* 3축으로 아래를 비교하면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/dx11/dx11-12-5.webp"/>
</p>

---

## 3D Triangle

### Point in Triangle

```
    c
    /\
   /  \
  /    \
 /  d   \
a-------- b
```

* d점이 Triangle내에 있는지 확인방법
* ab x ad의 방향과 ad x ac의 방향이 일치하는지 확인
* 이런식으로 세 변을 확인한다

```cpp
Vec3 a = t.a - p;
Vec3 b = t.b - p;
Vec3 c = t.c - p;

Vec3 normPBC = b.Cross(c);
Vec3 normPCA = c.Cross(a);
Vec3 normPAB = a.Cross(b);

if (normPBC.Dot(normPCA < 0.0f))
  return false;
else if (normPBC.Dot(normPAB < 0.0f))
  return false;
return true;
```

* Barycentric으로 구하는 방법도 있다.(미정리)

