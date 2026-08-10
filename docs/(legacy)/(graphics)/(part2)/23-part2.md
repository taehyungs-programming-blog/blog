---
layout: default
title: "23. Rim Lighting"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 이론

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/15_rimLighting)

* **Rim Lighting** - 역광효과라 생각하면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-13-1.jpg"/>
</p>

* 그림으로 봐선 가장자리를 밝게 바꿔주면 될꺼 같은데?? -> 정답
* 그럼 가장자리를 어떻게 찾느냐? -> 이거만 해결하면 된다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-13-2.jpg"/>
</p>

* 물체의 normal vector(n1, n2)와 눈과 이은 vector(e1, e2)가 유사하다면 물체의 표면이고, 90도 이상 틀어진다면 물체의 가장자리라 생각할 수 있다.

```cpp
float4 main(PixelShaderInput input) : SV_TARGET
{
    // ...


    // Rim Lighting
    // OpenGL SuperBible 7th Edition, Ch13. Rendering Techniques
    
    // Smoothstep
    // https://thebookofshaders.com/glossary/?search=smoothstep

    // normal과 eye vector의 dot product
    color += (1.0 - dot(input.normalWorld, toEye)) * rimColor * rimStrength; // 뒤의 rimColor, rimStrength는 부가적인 애들임

    return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.texcoord) : float4(color, 1.0);
}

```

---

* 추가, 가장자리만 도드라지게 Rim Light을 줘보자

```cpp
float rim = (1.0 - dot(input.normalWorld, toEye));
rim = pow(abs(rim), rimPower);

color += rim * rimColor * rimStrength;
```