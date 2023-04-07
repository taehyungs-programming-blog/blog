---
layout: default
title: "12. Super Sampling"
parent: "(Graphics Basic Concept)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code🌎](https://github.com/Arthur880708/DirectX11-Examples/tree/master/14_SuperSampling)

## Sampling이 필요한 이유

* 적절한 Sampling 없이 이미지를 그린다면 아래 그림과 같이 계단현상을 피할순 없다.
    * 물론 해상도를 높이면 해결이 가능하지만 그건 좀;; 무식하지 않나?
* 어떻게 Sampling을 좀 추가하여 계단현상을 피할 방법이 없을까? -> **Super Sampling**

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-12-2.png"/>
</p>

## Super Sampling?

* 현재는 하나의 Pixel에서 하나의 Ray를 쏜다.
* 만약 하나의 Pixel에서 여러개의 Ray를 쏜다면?
    * ??? 말이 되나? Pixel하나에 여러 색을 입힌단 말인가?
    * Nope 여러 Ray를 쏘고 평균을 낸다는 말이다.
* 실제로 다양한 Super Sampling 방법이 존재한다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-12-1.png"/>
</p>

---

## 구현

```cpp
vec3 traceRay2x2(vec3 eyePos, vec3 pixelPos, const float dx, const int recursiveLevel)
{
    if (recursiveLevel == 0)
    {
        Ray myRay{ pixelPos, glm::normalize(pixelPos - eyePos) };
        return traceRay(myRay);
    }

    const float subdx = 0.5f * dx;

    vec3 pixelColor(0.0f);
    pixelPos = vec3(pixelPos.x - subdx * 0.5f, pixelPos.y - subdx * 0.5f, pixelPos.z);

    for (int j = 0; j < 2; j++)
        for (int i = 0; i < 2; i++)
        {
            vec3 subPos(pixelPos.x + float(i) * subdx, pixelPos.y + float(j) * subdx, pixelPos.z);
            Ray sub_ray{ subPos, glm::normalize(subPos - eyePos) };
            pixelColor += traceRay2x2(eyePos, subPos, subdx, recursiveLevel - 1);
        }

    return pixelColor * 0.25f;
}
```
