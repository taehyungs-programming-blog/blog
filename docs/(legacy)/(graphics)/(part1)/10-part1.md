---
layout: default
title: "10. 무게 중심 좌표계"
parent: "(Graphics Basic Concept)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code🌎](https://github.com/Arthur880708/DirectX11-Examples/tree/master/12_barycentric_coord)

## 무게 중심 좌표계(Barycentric coordinates)

* Vertex에 색을 지정하고 Vertex의 내부를 적절한 색으로 칠하는 기법
    * 아래와 같은 색상을 어떻게 입히냐 문제이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-10-1.png"/>
</p>

```
v1       
--------- v2
|       /
|      /
|  p  /
|    /
|   /
|  /
| /
|/
v0
```

* `p`가 무게 중심에 있다면
* `p`를 `p = (v0 + v1 + v2) / 3`이라 표현이 가능하다
    * `p = (1 / 3)*v0  + (1 / 3)*v1 + (1 / 3)*v2`

* `p`가 딱 무게 중심에 있는게 아니라면? (특정 Vector에 치우쳐있다면?)
    * `p = w0*v0  + w1*v1 + w2*v2`
    * 각 Vector의 상수(`w0~w2`)가 달라질 것이다.

* 어떻게 구할까? `p`의 위치에 따라 셋으로 쪼개진 삼각형의 크기가 달라진다.
* 삼각형 크기는 Cross Product으로 구할수 있다
* 여기까지만 이해하고 구현을 보자

---

## 구현

```cpp
// 광선이 물체에 닿으면 그 물체의 색 반환
vec3 traceRay(Ray &ray)
{
    // Render first hit
    const auto hit = FindClosestCollision(ray);

    if (hit.d >= 0.0f)
    {
        glm::vec3 pointColor;

        // Ambient
        pointColor = hit.obj->amb;

        if (hit.obj == this->tempObject) // 임시로 삼각형만 색을 직접 결정
        {
            // Barycentric coordinates를 이용한 색 섞기(interpolation)
            const auto color0 = vec3(1.0f, 0.0f, 0.0f);
            const auto color1 = vec3(0.0f, 1.0f, 0.0f);
            const auto color2 = vec3(0.0f, 0.0f, 1.0f);

            const float w0 = hit.w[0]; // hit.w.x
            const float w1 = hit.w[1]; // hit.w.y
            const float w2 = 1.0f - w0 - w1;

            pointColor = color0 * w0 + color1 * w1 + color2 * w2;
        }

        const vec3 dirToLight = glm::normalize(light.pos - hit.point);

        // 그림자 꺼놓음
        // Ray shadowRay = { hit.point + dirToLight * 1e-4f, dirToLight };
        // if (FindClosestCollision(shadowRay).d < 0.0f)
        {
            const float diff = glm::max(dot(hit.normal, dirToLight), 0.0f);

            // Specular
            const vec3 reflectDir = dirToLight - hit.normal * 2.0f * dot(dirToLight, hit.normal);
            const float specular = glm::pow(glm::max(glm::dot(ray.dir, reflectDir), 0.0f), hit.obj->alpha);

            pointColor += diff * hit.obj->dif;
            pointColor += hit.obj->spec * specular;
        }

        return pointColor;
    }

    return vec3(0.0f);
}
```

```cpp
// 참고: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
bool IntersectRayTriangle(const vec3 &orig, const vec3 &dir,
                            const vec3 &v0, const vec3 &v1,
                            const vec3 &v2, vec3 &point, vec3 &faceNormal,
                            float &t, float &w0, float &w1)
{
    // ...

    /* 3. 그 충돌 위치가 삼각형 안에 들어 있는 지 확인 */

    // 작은 삼각형들 3개의 normal 계산
    // 방향만 확인하면 되기 때문에 normalize() 생략 가능
    const vec3 cross0 = glm::cross(point - v2, v1 - v2);
    const vec3 cross1 = glm::cross(point - v0, v2 - v0);
    const vec3 cross2 = glm::cross(v1 - v0, point - v0);

    if (dot(cross0, faceNormal) < 0.0f)
        return false;
    if (dot(cross1, faceNormal) < 0.0f)
        return false;
    if (dot(cross2, faceNormal) < 0.0f)
        return false;

    // Barycentric coordinates 계산
    // 텍스춰링(texturing)에서 사용
    // 아래에서 cross product의 절대값으로 작은 삼각형들의 넓이 계산

    const float area0 = glm::length(cross0) * 0.5f;
    const float area1 = glm::length(cross1) * 0.5f;
    const float area2 = glm::length(cross2) * 0.5f;

    const float areaSum = area0 + area1 + area2;

    // 기호에 alpha, beta, gamma 또는 u, v, w 등을 사용하기도 함
    w0 = area0 / areaSum;
    w1 = area1 / areaSum;

    return true;
}
```