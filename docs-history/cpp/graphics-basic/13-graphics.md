---
layout: default
title: "13. 빛의 반사"
parent: "(Graphics Basic 🎡)"
grand_parent: C++
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/tree/17/15_Reflection)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-13-1.jpg"/>
</p>

* 반사란게 대략 위 그림과 같은것인데 ...

* 사실 이론적으론 간단하다 
    * Screen View(Pixel)에서 Ray를 쏘고 그 맞은 Ray에서 다시 Ray를 쏴서 맞는곳이 있는지 체크한다.

---

## 구현

```cpp
vec3 traceRay(Ray &ray, const int recurseLevel)
{
    if (recurseLevel < 0)
        return vec3(0.0f);

    // Render first hit
    const auto hit = FindClosestCollision(ray);

    if (hit.d >= 0.0f)
    {
        glm::vec3 color(0.0f);

        const vec3 dirToLight = glm::normalize(light.pos - hit.point);

        // 그림자 생략
        // Ray shadowRay = { hit.point + dirToLight * 1e-4f, dirToLight };
        // if (FindClosestCollision(shadowRay).d < 0.0f)
        {
            glm::vec3 phongColor(0.0f);

            const float diff = glm::max(dot(hit.normal, dirToLight), 0.0f);
            const vec3 reflectDir = 2.0f * hit.normal * dot(dirToLight, hit.normal) - dirToLight;
            const float specular = glm::pow(glm::max(glm::dot(-ray.dir, reflectDir), 0.0f), hit.obj->alpha);

            if (hit.obj->ambTexture)
            {
                phongColor += hit.obj->amb * hit.obj->ambTexture->SampleLinear(hit.uv);
            }
            else
            {
                phongColor += hit.obj->amb;
            }

            if (hit.obj->difTexture)
            {
                phongColor += diff * hit.obj->dif * hit.obj->difTexture->SampleLinear(hit.uv);
            }
            else
            {
                phongColor += diff * hit.obj->dif;
            }

            phongColor += hit.obj->spec * specular;

            color += phongColor * (1.0f - hit.obj->reflection 
                    - hit.obj->transparency // 투명할 경우 투과되는것을 반영
                    );

            if (hit.obj->reflection)
            {
                // 여기에 반사 구현
                // 수치 오류 주의
                // 반사광이 반환해준 색을 더할 때의 비율은 hit.obj->reflection

                const vec3 reflectedDirection = glm::normalize(hit.normal * 2.0f * dot(-ray.dir, hit.normal) + ray.dir);
                Ray reflection_ray{ hit.point + reflectedDirection * 1e-4f, reflectedDirection };
                color += traceRay(reflection_ray, recurseLevel - 1) * hit.obj->reflection;
            }

            if (hit.obj->transparency)
            {
                // 투명한 물체의 굴절 처리
            }
        }

        return color;
    }

    return vec3(0.0f);
}
```
