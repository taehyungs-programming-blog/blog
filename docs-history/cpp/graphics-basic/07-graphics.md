---
layout: default
title: "07. 원근법 적용"
parent: "(Graphics Basic 🎡)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 원근법 적용 + 가까운 물체가 그려지게 적용

* [Clone Code🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/11/09)

```cpp
Raytracer(const int &width, const int &height)
    : width(width), height(height)
{
    // 스크린으로부터 거리가 다른 구 3개
        // z값이 모두 다름을 기억하자
    auto sphere1 = make_shared<Sphere>(vec3(0.5f, 0.0f, 0.5f/*z*/), 
                                        0.4f, vec3(0.5f, 0.5f, 0.5f));
    auto sphere2 = make_shared<Sphere>(vec3(0.0f, 0.0f, 1.0f/*z*/), 
                                        0.4f, vec3(0.5f, 0.5f, 0.5f));
    auto sphere3 = make_shared<Sphere>(vec3(-0.5f, 0.0f, 1.5f/*z*/), 
                                        0.4f, vec3(0.5f, 0.5f, 0.5f));

    // ...

    // 일부러 역순으로 추가
        // 목표하는 것은 들어간 순서로 출력되는게 아니라
        // z값에 다라 출력되는것을 원한다
    objects.push_back(sphere3);
    objects.push_back(sphere2);
    objects.push_back(sphere1);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-7-2.png"/>
</p>

```cpp
// 그럴만도 한게 현재는 Collision 체크를 가장먼저 hit되면 출력하게 되어있음.

Hit FindClosestCollision(Ray& ray)
{
    Hit closest_hit = Hit{ -1.0, dvec3(0.0), dvec3(0.0) };

    for (int l = 0; l < objects.size(); l++)
    {
        auto hit = objects[l]->CheckRayCollision(ray);

        if (hit.d >= 0.0f)
        {
            hit.obj = objects[l];
            return hit;
        }
    }

    return closest_hit;
}
```

* 가장 가까이 있는 물체를 리턴하게 만들어야한다.
    * 거기다 원근감까지!!

---

## 가까이 있는 물체리턴은 쉽다

```cpp
Hit FindClosestCollision(Ray& ray)
{
    float closestD = 1000.0; //inf
    Hit closest_hit = Hit{ -1.0, dvec3(0.0), dvec3(0.0) };

    for (int l = 0; l < objects.size(); l++)
    {
        auto hit = objects[l]->CheckRayCollision(ray);

        if (hit.d >= 0.0f)
        {
            if (hit.d < closestD)
            {
                closestD = hit.d;
                closest_hit = hit;
                closest_hit.obj = objects[l];
            }
        }
    }

    return closest_hit;
}
```

---

## 이제 원근감차례(멀리있는건 작게 가까이 있는건 크게!)

```cpp
void Render(std::vector<glm::vec4>& pixels)
{
    std::fill(pixels.begin(), pixels.end(), vec4{ 0.0f, 0.0f, 0.0f, 1.0f });

    const vec3 eyePos(0.0f, 0.0f, -1.5f);

#pragma omp parallel for
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            const vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));

            // 현재는 ray를 쏠때 아래와 같이 0, 0, 1로 쏘지만 -> Orthographic projection (정투영)
            // 이제는 눈을 기준으로 ray를 쏘게 수정할 예정 -> perspective projection (원근투영)
            const auto rayDir = vec3(0.0f, 0.0f, 1.0f);
            Ray pixelRay{ pixelPosWorld, rayDir };

            pixels[i + width * j] = vec4(glm::clamp(traceRay(pixelRay), 0.0f, 1.0f), 1.0f);
        }
```

```cpp
void Render(std::vector<glm::vec4>& pixels)
{
    std::fill(pixels.begin(), pixels.end(), vec4{ 0.0f, 0.0f, 0.0f, 1.0f });

    // eysPos는 미리 정의된 값이다
    const vec3 eyePos(0.0f, 0.0f, -1.5f);

#pragma omp parallel for
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            const vec3 pixelPosWorld = TransformScreenToWorld(vec2(i, j));

            // 광선의 방향 벡터
            // 스크린에 수직인 z방향, 절대값 1.0인 유닉 벡터
            // Orthographic projection (정투영) vs perspective projection (원근투영)

            // 원근법 적용.
            Ray pixelRay{ pixelPosWorld, normalize(pixelPosWorld - eyePos)};

            pixels[i + width * j] = vec4(glm::clamp(traceRay(pixelRay), 0.0f, 1.0f), 1.0f);
        }
}
```

* 이런 궁금증이 생길수 있음
    * 어떻게 z축이 멀리있는게 작게 그려지는것인가??
    * 너무 신기한데?

* 눈에서 픽셀의 방향으로 ray를 쏜다고 생각하자
* `(pixelPosWorld - eyePos)`

* 기존과의 차이는 뭐냐?
    * 우선 3D공간에 세 물체를 놓은건 받아들이고 
    * 그 물체로 기존은 pixel마다 카메라(눈)가 있는개념이였다면
    * 이제는 카메라는 하나이고 pixel로 ray를 쏘는 개념이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-7-4.png"/>
</p>