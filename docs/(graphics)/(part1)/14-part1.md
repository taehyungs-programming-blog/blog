---
layout: default
title: "14. 투명도 적용, 빛의 굴절 적용"
parent: "(Graphics Basic Concept)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code🌎](https://github.com/Arthur880708/DirectX11-Examples/tree/master/16_tranceparency)

* 적용하고자 하는것은 투명도(Transparency)가 있는 물체에 빛이 만나면 빛이 굴절되어 나가는것을 반영하고자 한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-14-1.png"/>
</p>

* 투명도가 있는 물체를 만나면 굴절을 넣은 Ray를 생성 후 다시 Ray를 쏘는식으로 만들면 되겠군?

---

## 사전지식

```
* - dot product

a * (-b) = -(a * b)
a * b = b * a

// a, b가 normalize일때
a * b = cos(r)
// -> cos(r)를 구할때 a b dot product로 간단히 구할수 있음.

cos^2(r) + sin^2(r) = 1
sin^2(r) = 1 - cos^2(r)
sin(r) = sqrt(1 - cos^2(r))
sin(r) = sqrt(1 - (a * b)^2)
```

* 굴절률
    * 물리학자들이 구한값이기에 의심은 말자...
    * 공기 -> 유리 : 1.5
    * 유리 -> 공기 : 1 / 1.5

---

## 구현

```cpp
// 광선이 물체에 닿으면 그 물체의 색 반환
vec3 traceRay(Ray &ray, const int recurseLevel)
{
    // ...

        // 참고
        // https://samdriver.xyz/article/refraction-sphere (그림들이 좋아요)
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel (오류있음)
        // https://web.cse.ohio-state.edu/~shen.94/681/Site/Slides_files/reflection_refraction.pdf (슬라이드가 보기 좋지는 않지만 정확해요)
        if (hit.obj->transparency)
        {
            const float ior = 1.5f; // Index of refraction (유리: 1.5, 물: 1.3)

            float eta; // sinTheta1 / sinTheta2
            vec3 normal;

            if (glm::dot(ray.dir, hit.normal) < 0.0f) // 밖에서 안에서 들어가는 경우 (예: 공기->유리)
            {
                eta = ior;
                normal = hit.normal;
            }
            else // 안에서 밖으로 나가는 경우 (예: 유리->공기)
            {
                eta = 1.0f / ior;
                normal = -hit.normal;
            }

                const float cosTheta1 = glm::dot(-ray.dir, normal);
                const float sinTheta1 = sqrt(1.0f - cosTheta1 * cosTheta1);	// cos*2 + sin*2 = 1
                const float sinTheta2 = sinTheta1 / eta ;
                const float cosTheta2 = sqrt(1.0f - sinTheta2 * sinTheta2);

                const vec3 m = glm::normalize(dot(normal, -ray.dir) * normal + ray.dir);
                const vec3 a = m * sinTheta2;
                const vec3 b = -normal * cosTheta2;
                const vec3 refractedDirection = glm::normalize(a + b); // transmission

                Ray refractionRay{ hit.point + refractedDirection * 0.001f, refractedDirection };
                color += traceRay(refractionRay, recurseLevel - 1) * hit.obj->transparency;

            // Fresnel 효과는 생략되었습니다.
        }

        return color;
    }

    return vec3(0.0f);
}
```