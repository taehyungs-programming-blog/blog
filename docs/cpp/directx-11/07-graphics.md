---
layout: default
title: "07. 조명 효과 넣기"
parent: "(DirectX 11 🎡)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/10/08)

* Phong Reflection Model을 적용할 예정
    * Ambient - 물체에 전반적으로 들어오는 빛을 계산
    * Diffuse - 빛이 직사광으로 들어오는지 비스듬하게 들어오는지 계산
    * Specular - 정반사된 빛을 계산
* 세개를 반영하면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-7-1.png"/>
</p>

```cpp
// 광선이 물체에 닿으면 그 물체의 색 반환
vec3 traceRay(Ray &ray)
{
    auto hit = sphere->IntersectRayCollision(ray);

    if (hit.d < 0.0f)
    {
        return vec3(0.0f);
    }
    else
    {
        // return sphere->color * hit.d; // <- 앞에서 사용했던 색 결정

        // 여기서 퐁 모델(Phong reflection model)으로 조명 효과 계산!
        // 참고 자료
        // https://en.wikipedia.org/wiki/Phong_reflection_model
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/phong-shader-BRDF

        // Diffuse
        const vec3 dirToLight = glm::normalize(light.pos - hit.point);
        const float diff = glm::max(glm::dot(hit.normal, dirToLight), 0.0f);

        // Specular
        const vec3 reflectDir = 2.0f * dot(hit.normal, dirToLight) * hit.normal - dirToLight;
        const float specular = glm::pow(glm::max(glm::dot(-ray.dir, reflectDir), 0.0f), sphere->alpha);

        return sphere->amb + sphere->diff * diff + sphere->spec * specular * sphere->ks;
        // return sphere->diff * diff;
        // return sphere->spec * specular * sphere->ks;
    }
}
```

---

## 원근법 적용 + 가까운 물체가 그려지게 적용

* [Clone Code🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/11/09)

```cpp
Raytracer(const int &width, const int &height)
    : width(width), height(height)
{
    // 스크린으로부터 거리가 다른 구 3개
        // z값이 모두 다름을 기억하자
    auto sphere1 = make_shared<Sphere>(vec3(0.5f, 0.0f, 0.5f), 0.4f, vec3(0.5f, 0.5f, 0.5f));
    auto sphere2 = make_shared<Sphere>(vec3(0.0f, 0.0f, 1.0f), 0.4f, vec3(0.5f, 0.5f, 0.5f));
    auto sphere3 = make_shared<Sphere>(vec3(-0.5f, 0.0f, 1.5f), 0.4f, vec3(0.5f, 0.5f, 0.5f));

    // 일부러 역순으로 추가
    objects.push_back(sphere3);
    objects.push_back(sphere2);
    objects.push_back(sphere1);
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-7-2.png"/>
</p>

* 현재는 z에 상관없이 제일 먼저 들어간 object를 먼저 그린다

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