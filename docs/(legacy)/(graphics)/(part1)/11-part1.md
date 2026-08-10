---
layout: default
title: "11. Texturing"
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

* [Clone Code🌎](https://github.com/Arthur880708/DirectX11-Examples/tree/master/13_texturing)

## Texture Coordinate (테스쳐 좌표)

* 내가 입힐 이미지의 좌표와 입혀질 Object의 좌표계를 맞추는 것을 의미한다.
    * 가령 아래의 그림과 같이 네모박스에 나무질감의 이미지를 넣은것

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-11-1.jpg"/>
</p>

* 그럼 핵심은 Vertex(Object)의 좌표와 이미지의 좌표를 어떻게 맞추는가이다.

* 간단히 말하면 Vertex에 Texture좌표를 Mapping한다.
* Texture 좌표를 uv라 정의하고, v0에 uv(0, 0) v1에 uv(1, 0) 뭐 이런식 ..

* 아니 그러면 Vertex 사이의 Pixel의 색상은 어떻게 정의하나?
* 앞에서 배운 **무게 중심 좌표계(Barycentric coordinates)**를 활용한다.

---

## 구현

```cpp
Raytracer(const int &width, const int &height)
    : width(width), height(height)
{
    // ...

    auto imageTexture = std::make_shared<Texture>(4, 4, textureImage);
    // auto imageTexture = std::make_shared<Texture>("shadertoy_abstract1.jpg");

    auto square = make_shared<Square>(vec3(-2.0f, 2.0f, 2.0f), vec3(2.0f, 2.0f, 2.0f), vec3(2.0f, -2.0f, 2.0f), vec3(-2.0f, -2.0f, 2.0f),
            // 기존과 차이점은 Vertex에 Texture Coordinate을 넣어준다
            vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f));

```

* 두 가지 방법으로 구현해 볼 예정이다.
    * **Point Sampling**
    * **Linear Sampling**

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-11-2.png"/>
</p>

```cpp
vec3 SamplePoint(const vec2 &uv) // Nearest sampling이라고 부르기도 함
{
    // 텍스춰 좌표의 범위 uv [0.0, 1.0] x [0.0, 1.0]
    // 이미지 좌표의 범위 xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
    // 배열 인덱스의 정수 범위 ij [0, width-1] x [0, height - 1]

    vec2 xy = uv * vec2((float(width), float(height))) - vec2(0.5);
    int i = glm::round(xy.x);
    int j = glm::round(xy.y);

    return GetClamped(i, j);
}
```

```cpp
vec3 SampleLinear(const vec2& uv)
{
    // 텍스춰 좌표의 범위 uv [0.0, 1.0] x [0.0, 1.0]
    // 이미지 좌표의 범위 xy [-0.5, width - 1 + 0.5] x [-0.5, height - 1 + 0.5]
    // std::cout << floor(-0.3f) << " " << int(-0.3f) << std::endl; // -1 0

    const vec2 xy = uv * vec2(width, height) - vec2(0.5f);
    const int i = int(floor(xy.x));
    const int j = int(floor(xy.y));
    const float dx = xy.x - float(i);
    const float dy = xy.y - float(j);

    return InterpolateBilinear(dx, dy, GetClamped(i, j), GetClamped(i + 1, j), 
        GetClamped(i, j + 1), GetClamped(i + 1, j + 1));

/*
    vec3 InterpolateBilinear(
        const float &dx,
        const float &dy,
        const vec3 &c00,
        const vec3 &c10,
        const vec3 &c01,
        const vec3 &c11)
    {
        vec3 a = c00 * (1.0f - dx) + c10 * dx;
        vec3 b = c01 * (1.0f - dx) + c11 * dx;

        return a * (1.0f - dy) + b * dy;
    }
*/
}
```
