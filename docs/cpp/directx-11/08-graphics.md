---
layout: default
title: "08. 삼각형그리기 + 광선의 충돌"
parent: "(DirectX 11 🎡)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 삼각형을 그려보자

* [Clone Code🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/12/10_Triangle)

```cpp
// 카메라의 Ray가 Hit할 경우 그리면 되니
// Ray Collision에서 Hit 여부를 체크

virtual Hit CheckRayCollision(Ray &ray)
{
    Hit hit = Hit{-1.0, vec3(0.0), vec3(0.0)};

    vec3 point, faceNormal;
    float t, u, v;


    // Ray의 시작위치와 방향, 그리고 세 Vertex(정점)을 넘긴다.
    // 리턴으론 point, faceNormal, t, u, v가 넘어온다
        // (아직은 그냥 받아들이자.)
    if (IntersectRayTriangle(ray.start, ray.dir, this->v0, this->v1,
                                this->v2, point, faceNormal, t, u, v))
    {
        hit.d = t;
        hit.point = point; // ray.start + ray.dir * t;
        hit.normal = faceNormal;

        // 텍스춰링(texturing)에서 사용
        // hit.uv = uv0 * u + uv1 * v + uv2 * (1.0f - u - v);
    }

    return hit;
}
```

---

## 잠깐 Vector 복습

* **Dot Product** - 내적, 두 Vector의 연관관계를 체크
    * 두 벡터가 같은 방향이다. cos(0)이 되며 -> 1
    * 두 벡터가 90도 방향이다. cos(90)이 되며 -> 0
    * 두 벡터가 180도 방향이다. cos(180)이 되며 -> -1
* **Cross Product** - 외적, 두 Vector의 평면을 정의하고 그 평면에 수직인 Vector를 나타낸다.
    * Example) 두 벡터로 만들어진 삼각형의 넓이
        * `| a x b | / 2`

---

## 자! 그럼 Collision은 어떻게 계산할까?

* 우리가 알고있는 것은 
    * 세 정점(`v0~v2`)
    * Ray의 원점(`o`)
    * 원점에서 쏜 Ray(`d`)
* 그럼 세 정점으로 만들어진 평면에 Ray가 맞은 점을 `p`라고 한다면
* `p = o + td` -> 원점(`o`)에서 `d`방향으로 `t`상수만큼 움직여 쏘아 맞았기에 이렇게 표현가능
* 그럼 `p`가 세 정점(`v0~v2`) 안에 있었는지만 확인하면 된다.
    * 세 정점을 알기에 정점으로 만들어진 평면의 Normal Vector(`n`)를 구한다(Cross Product 하면되겠지?)
    * 더 상세한 증명은 아래 페이지 참조...

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-8-1.png"/>
</p>

```cpp
// 수학 프로그래밍을 좋아하시는 분들은 직접 구현해보시면 좋고,
// 대부분은 개념만 이해해두시고 활용하는 방향으로 접근하셔도 충분합니다.
// 잘 이해가 가지 않는다면 여러 자료로 교차 검증하면서 공부하시는 방법도
// 좋습니다. 참고:
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
bool IntersectRayTriangle(const vec3 &orig, const vec3 &dir,
                            const vec3 &v0, const vec3 &v1,
                            const vec3 &v2, vec3 &point, vec3 &faceNormal,
                            float &t, float &u, float &v)
{
    /*
        * 기본 전략
        * - 삼각형이 놓여있는 평면과 광선의 교점을 찾고,
        * - 그 교점이 삼각형 안에 있는지 밖에 있는지를 판단한다.
        */

    /* 1. 세 정점이 놓여 있는 평면의 수직 벡터 계산 */
    faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    //주의: 삼각형의 넓이가 0일 경우에는 계산할 수 없음

    // 삼각형 뒷면을 그리고 싶지 않은 경우 (Backface culling)
    if (dot(-dir, faceNormal) < 0.0f) return false;

    // 평면과 광선이 수평에 매우 가깝다면 충돌하지 못하는 것으로 판단
    if (glm::abs(dot(dir, faceNormal)) < 1e-2f) return false; 
        // t 계산시 0으로 나누기 방지

    /* 2. 광선과 평면의 충돌 위치 계산 */
        // 이 증명은 참고페이지 참고
    t = (dot(v0, faceNormal) - dot(orig, faceNormal)) / (dot(dir, faceNormal));

    // 광선의 시작점 이전에 충돌한다면 렌더링할 필요 없음
    if (t < 0.0f) return false;

    point = orig + t * dir; // 충돌점

    /* 3. 그 충돌 위치 p가 
    세 정점 삼각형 안에 들어 있는 지 확인 */

    // 세 정점을 이용해
    // 작은 삼각형들 3개의 normal 계산
    /*
        | \
        |  \
        |   \
        | p  \ <- p점을 중심으로 다시 삼각형 세개로 나눈다
        |     \
        -------
    */
    const vec3 normal0 = glm::normalize(glm::cross(point - v2, v1 - v2));
    const vec3 normal1 = glm::normalize(glm::cross(point - v0, v2 - v0));
    const vec3 normal2 = glm::normalize(glm::cross(v1 - v0, point - v2));
    // 방향만 확인하면 되기 때문에 normalize() 생략 가능
    // 아래에서 cross product의 절대값으로 작은 삼각형들의 넓이 계산
        // 이 증명은 역시 페이지 참조 ...
    if (dot(normal0, faceNormal) < 0.0f) return false;
    if (dot(normal1, faceNormal) < 0.0f) return false;
    if (dot(normal2, faceNormal) < 0.0f) return false;

    // Barycentric coordinates 계산
    // 이후에 텍스춰링(texturing)에서 사용하며 구현예정
    // u = ...
    // v = ...

    return true;
}
```