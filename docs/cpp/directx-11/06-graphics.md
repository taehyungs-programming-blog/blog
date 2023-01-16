---
layout: default
title: "06. 3D 원 그리기"
parent: "(DirectX 11 🎡)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Code Clone 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/9/07)

```cpp
void Render(std::vector<glm::vec4> &pixels)
{
    // 우선 모든 Pixel을 검은색으로 칠한다.
    std::fill(pixels.begin(), pixels.end(), vec4{0.0f, 0.0f, 0.0f, 1.0f});

#pragma omp parallel for
    // 각 Pixcel을 돌면서
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            // (1) Pixel을 World좌표계로 변환한다.(z축은 0으로 고정)
            const vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));

            // 광선의 방향 벡터
            // 스크린에 수직인 z방향, 절대값 1.0인 유닉 벡터
            // Orthographic projection (정투영) vs perspective projection (원근투영)
            const auto rayDir = vec3(0.0f, 0.0f, 1.0f);

            Ray pixelRay{pixelPosWorld, rayDir};


            // (2)
            // index에는 size_t형 사용 (index가 음수일 수는 없으니까)
            // traceRay()의 반환형은 vec3 (RGB), A는 불필요
            pixels[size_t(i + width * j)] = vec4(traceRay(pixelRay), 1.0f);
        }
}
```

```cpp
// (1)
glm::vec3 TransformScreenToWorld(glm::vec2 posScreen)
{
    const float xScale = 2.0f / (this->width - 1);
    const float yScale = 2.0f / (this->height - 1);
    const float aspect = float(this->width) / this->height;

    // 3차원 공간으로 확장 (z좌표는 0.0으로 가정)
    return glm::vec3((posScreen.x * xScale - 1.0f) * aspect, -posScreen.y * yScale + 1.0f, 0.0f);
}
```

```cpp
// 광선이 물체에 닿으면 그 물체의 색 반환
vec3 traceRay(Ray &ray)
{
    // hit했다면
    const Hit hit = sphere->IntersectRayCollision(ray);

    if (hit.d < 0.0f)
    {
        return vec3(0.0f);
    }
    else
    {
        // sphere의 색상에 거리값에 대해 반영
        return sphere->color * hit.d; // 깊이를 곱해서 입체감 만들기
    }
}
```

* 핵심 `IntersectRayCollision` 구현하기

```cpp
// Wikipedia Line–sphere intersection
// https://en.wikipedia.org/wiki/Line-sphere_intersection
Hit IntersectRayCollision(Ray &ray)
{
    Hit hit = Hit{-1.0f, vec3(0.0f), vec3(0.0f)}; // d가 음수이면 충돌을 안한 것으로 가정

    /*
        * hit.d = ... // 광선의 시작점으로부터 충돌지점까지의 거리 (float)
        * hit.point = ... // 광선과 구가 충돌한 지점의 위치 (vec3)
        * hit.normal = .. // 충돌 지점에서 구의 단위 법선 벡터(unit normal vector)
        */

    // const float a = glm::dot(ray.dir, ray.dir); // dir이 unit vector라면 a는 1.0f라서 생략 가능

    // 식은 Wiki의 설명을 참고

    const float b = 2.0f * glm::dot(ray.dir, ray.start - this->center);
    const float c = dot(ray.start - this->center, ray.start - this->center) - this->radius * this->radius;
    const float nabla = b * b / 4.0f - c;

    if (nabla >= 0.0f)
    {
        const float d1 = -b / 2.0f + sqrt(nabla);
        const float d2 = -b / 2.0f - sqrt(nabla);
        hit.d = glm::min(d1, d2);
        hit.point = ray.start + ray.dir * hit.d;
        hit.normal = glm::normalize(hit.point - this->center);
    }

    return hit;
}
```