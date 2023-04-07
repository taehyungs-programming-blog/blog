---
layout: default
title: "14. Texturing"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/5_texturing)

* Texture를 입혀보자!

```cpp
// 이런식으로 pixel shader에 texture를 넘겨쓴다
Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer PixelShaderConstantBuffer : register(b0) { float xSplit; };

struct PixelShaderInput {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET {

    return input.texcoord.x > xSplit
               ? g_texture0.Sample(g_sampler, input.texcoord)
               : float4(1.0, 0.0, 0.0, 1.0);
}
```

* 어떻게 쓰는지 c++ 코드를 좀 더 보자면

```cpp
// Texturing용 변수 선언
ComPtr<ID3D11Texture2D> m_texture;
ComPtr<ID3D11ShaderResourceView> m_textureResourceView;
ComPtr<ID3D11SamplerState> m_samplerState;

// 사용법은 쉽기에 코드를 볼 것.
```

---

## 실습(Texture를 2개 적용해 보자)

* C++코드는 쉽기에 shader만 적자면

```cpp
Texture2D g_texture0 : register(t0);
Texture2D g_texture1 : register(t1);
SamplerState g_sampler : register(s0);

cbuffer PixelShaderConstantBuffer : register(b0) { float xSplit; };

struct PixelShaderInput {
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET{

    // return input.texcoord.x > xSplit
    //           ? g_texture0.Sample(g_sampler, input.texcoord)
    //           : float4(1.0, 0.0, 0.0, 1.0);

    // 이미지가 밀려나는듯한 효과주기
    // return g_texture0.Sample(g_sampler, input.texcoord + float2(xSplit,
    // 0.0));

    // 작은 이미지 여러개 만들기
    // return g_texture0.Sample(g_sampler, input.texcoord * 10.0);

    // 조명을 받는듯한 효과 넣기
    // float x = input.texcoord.x;
    // float y = input.texcoord.y;

    // float f = (x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5) - 0.3 * 0.3;
    // float4 color = g_texture0.Sample(g_sampler, input.texcoord);

    // if (f < 0.0)
    //    return color * 1.5;
    // else
    //    return color;

    // 텍스쳐 하나 더 쓰기
    return input.texcoord.x > xSplit
               ? g_texture0.Sample(g_sampler, input.texcoord)
               : g_texture1.Sample(g_sampler, input.texcoord);
}
```