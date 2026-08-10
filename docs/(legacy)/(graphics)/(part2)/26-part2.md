---
layout: default
title: "26. 이미지 기반 라이팅"
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

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/17_environmentMapping)

* 이미지 Lighting에도 Diffuse, Specular로 나눌수 있음.
    * 왼쪽이 Specular, 오른쪽이 Diffuse

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-16-1.png"/>
</p>

```cpp
float4 main(PixelShaderInput input) : SV_TARGET
{
    // ...

    // 쉽게 이해할 수 있는 간단한 구현입니다.
    // IBL과 다른 쉐이딩 기법(예: 퐁 쉐이딩)을 같이 사용할 수도 있습니다.
    // 참고: https://www.shadertoy.com/view/lscBW4
    
    float4 diffuse = g_diffuseCube.Sample(g_sampler, input.normalWorld);
    float4 specular = g_specularCube.Sample(g_sampler, reflect(-toEye, input.normalWorld));
    
    specular *= pow((specular.x + specular.y + specular.z) / 3.0, material.shininess);
    diffuse.xyz *= material.diffuse;
    specular.xyz *= material.specular;
    
    if(useTexture)
    {
        diffuse *= g_texture0.Sample(g_sampler, input.texcoord);
    }

    return diffuse + specular;
    
    // return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.texcoord) : float4(color, 1.0);
}
```

---

## CMFT Studio

* [Link](https://github.com/dariomanesku/cmftStudio)

* 위에서 만든 Diffuse, Specular Texture CubeMap을 만든다