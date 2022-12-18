---
layout: default
title: "07. 조명 효과 넣기"
parent: "(Graphics🖼)"
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