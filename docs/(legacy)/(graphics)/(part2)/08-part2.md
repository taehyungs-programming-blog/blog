---
layout: default
title: "08. Perspective Projection"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Git Clone 🌎](https://github.com/Arthur880708/DirectX11_Part2_Rasterization/tree/main/8_perspectiveprojection)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/d11/d11-8-1.jpg"/>
</p>

* 그런데 단순히 Perspective Projection을 적용시 왜곡이 발생하게 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/d11/d11-8-2.jfif"/>
</p>

* 간단히 말하면 Interpolation을 3D Object에서 해야하는데 Screen View에서 하다보니 발생한 문제이다.

```cpp
// ...

// 코드상으론 간단

float w0 = EdgeFunction(v1, v2, point) / area;
float w1 = EdgeFunction(v2, v0, point) / area;
float w2 = EdgeFunction(v0, v1, point) / area;

if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {

    // Perspective-Correct Interpolation
    // 논문
    // https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
    // 해설글
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes
    // OpenGL 구현
    // https://stackoverflow.com/questions/24441631/how-exactly-does-opengl-do-perspectively-correct-linear-interpolation

    const float z0 = this->vertexBuffer[i0].z + distEyeToScreen;
    const float z1 = this->vertexBuffer[i1].z + distEyeToScreen;
    const float z2 = this->vertexBuffer[i2].z + distEyeToScreen;

    if (this->usePerspectiveProjection &&
        this->usePerspectiveCorrectInterpolation) {

            // w0, w1, w2를 z0, z1, z2를 이용해서 보정
        w0 /= z0;
        w1 /= z1;
        w2 /= z2;

        const float wSum = w0 + w1 + w2;

        w0 /= wSum;
        w1 /= wSum;
        w2 /= wSum;
    }

// ...
```