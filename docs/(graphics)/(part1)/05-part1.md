---
layout: default
title: "05. 3D 원 그리기"
parent: "(Graphics Basic Concept)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Code Clone 🌎](https://github.com/Arthur880708/DirectX11-Examples/tree/master/07)

## 구조가 좀 수정된다.

```cpp
// 구조가 좀 수정됨
// Example.h

void Update()
{
    std::vector<glm::vec4> pixels(width * height, glm::vec4{0.8f, 0.8f, 0.8f, 1.0f});

    // Raytracer 클래스를 새로 만들어서 렌더링에 사용
    raytracer.Render(pixels);

    // 렌더링 결과를 GPU 메모리로 복사
    D3D11_MAPPED_SUBRESOURCE ms;
    deviceContext->Map(canvasTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, pixels.data(), pixels.size() * sizeof(glm::vec4));
    deviceContext->Unmap(canvasTexture, NULL);
}
```

```cpp
// 수정할 부분이 있다면 Raytracer의 Render 수정.

void Render(std::vector<glm::vec4> &pixels)
{
    std::fill(pixels.begin(), pixels.end(), vec4{0.0f, 0.0f, 0.0f, 1.0f});

#pragma omp parallel for
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {

/*
    * 여기부터 ray tracing
* TransformScreenToWorld에 Screen좌표계(2차원) 을 넣으면 
* World좌표계(3차원)이 나오게 된다.
* 해당 Screen 좌표계에 찍히는 World좌표계의 색상을 의미.
*/
            const vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));

/*
    * 광선의 방향 벡터
* 스크린에 수직인 z방향, 절대값 1.0인 유닉 벡터
* Orthographic projection (정투영) vs perspective projection (원근투영)

* 이렇게 생각하면 편하다,
* 모든 픽셀에 카메라가 찍히고 있음(이후에는 특정 카메라(눈)에서 찍을 예정)
*/
            const auto rayDir = vec3(0.0f, 0.0f, 1.0f);

            Ray pixelRay{pixelPosWorld, rayDir};

            // index에는 size_t형 사용 (index가 음수일 수는 없으니까)
            // traceRay()의 반환형은 vec3 (RGB), A는 불필요

            // ray를 만들어 해당 좌표계에 쏴준다 -> traceRay
            pixels[size_t(i + width * j)] = vec4(traceRay(pixelRay), 1.0f);
        }
}
```

---

* 위에서 그냥 z를 Ray(카메라)로 정의했는데 그러면 되나?

## Ray에 관하여

* 생각해 보면 어려운게 Screen 좌표계는 2차원으로
* 아래와 같이 구성되어 있음.

```
(y)
|
|
|
|
|
|
--------------------- (x)
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-5-1.png"/>
</p>

* 이렇게 생각하면 편하다. x, y좌표계의 수직으로 나가는 vector가 ray가 된다.
    * 외적하면되겠지??
* x, y 좌표계의 외적한 vector가 ray가 된다고 기억하자
* 그런데 사실상 x,y는 직교하고 있고 x,y좌표계 그 자체이기에 z가 ray가 된다.

```cpp
// 다시 살펴 보자면 ...

void Render(std::vector<glm::vec4> &pixels)
{
    std::fill(pixels.begin(), pixels.end(), vec4{0.0f, 0.0f, 0.0f, 1.0f});

#pragma omp parallel for
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {

            const vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));
            // rayDir에 z=1을 넣은 모습
            const auto rayDir = vec3(0.0f, 0.0f, 1.0f);

            // Ray에 Pixel의 World 좌표계 + ray의 방향을 넣고
            Ray pixelRay{pixelPosWorld, rayDir};

            // traceRay를 할경우 Pixel의 색상을 출력하는 
            // traceRay를 구현하는게 목표이다.
            pixels[size_t(i + width * j)] = vec4(traceRay(pixelRay), 1.0f);
```

---

```cpp
// 또한 ray가 hit된 거리에 따라 색상을 변경할 예정

// 광선이 물체에 닿으면 그 물체의 색 반환
vec3 traceRay(Ray &ray)
{

// ray가 물체와 충돌했는지 분석
    const Hit hit = sphere->IntersectRayCollision(ray);

    if (hit.d < 0.0f)
    {
        return vec3(0.0f);
    }
    else
    {
        return sphere->color * hit.d; 
        // 깊이를 곱해서 입체감 만들기
            // 이후엔 좀 더 이쁘게 나오게 수정예정
    }
}
```

```cpp
// 참고로 Sphere는

Raytracer(const int &width, const int &height)
    : width(width), height(height)
{
    // z축으로 0.5만큼 들어가 있고
    // 반지름은 0.4에
    // color는 흰색이다(1, 1, 1)

    sphere = make_shared<Sphere>(vec3(0.0f, 0.0f, 0.5f), 
                                0.4f, vec3(1.0f, 1.0f, 1.0f));
}
```

---

## Ray Collision 분석

* 충돌은 세 가지 경우가 있다.
    1. 충돌하지 않은경우 (근이 0개)
    2. 딱! 접접에 충돌한 경우 (근이 1개)
    3. 그냥 충돌한 경우 (근이 2개)
* 이걸 증명했다 -> nabla!
    * nabla의 증명은 ... 필요하다면 해볼 것.
* 결론적으로 말하면 nabla가 0보다 클시 근이 1개이상인 케이스인데

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-5-2.png"/>
</p>

```cpp
if (nabla >= 0.0f) 
{ 
    const float d1 = -b / 2.0f + sqrt(nabla); 
    const float d2 = -b / 2.0f - sqrt(nabla);

    // 접접이 2개인 케이스가 있어서 먼접 접한 d가(최소의d)가 distance가 됨 
    hit.d = glm::min(d1, d2);
    // ray의 시작점에서 dir을 더하고 distance를 곱할시 point 
    hit.point = ray.start + ray.dir * hit.d;
    // 접선벡터는 hitpoint의 normalize값 
    hit.normal = glm::normalize(hit.point - this->center); 
}
```

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

    // const float a = glm::dot(ray.dir, ray.dir); 
    // dir이 unit vector라면 a는 1.0f라서 생략 가능

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
