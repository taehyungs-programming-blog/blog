---
layout: default
title: "25. 환경 Mapping"
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

* 환경에 따라 물체에 환경을 반사되게 만들어 보자
* 만드는 방법
    * 물체에 표면에 Normal Vector에 Mapping된 환경의 색을 가져온다.
    * 눈에 정투영된 환경의 색을 가져온다.
    * 이 두가지만 반영하면 됨.

```cpp
#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

Texture2D g_texture0 : register(t0);
// TODO:
TextureCube g_textureCube1 : register(t1);

SamplerState g_sampler : register(s0);

cbuffer BasicPixelConstantBuffer : register(b0)
{
    float3 eyeWorld;
    bool useTexture;
    Material material;
    Light light[MAX_LIGHTS];
    float3 rimColor;
    float rimPower;
    float rimStrength;
    bool useSmoothstep;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 toEye = normalize(eyeWorld - input.posWorld);

    float3 color = float3(0.0, 0.0, 0.0);
    
    int i = 0;
    
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-for
    // https://forum.unity.com/threads/what-are-unroll-and-loop-when-to-use-them.1283096/
    
    [unroll] // warning X3557: loop only executes for 1 iteration(s), forcing loop to unroll
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        color += ComputeDirectionalLight(light[i], material, input.normalWorld, toEye);
    }
    
    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        color += ComputePointLight(light[i], material, input.posWorld, input.normalWorld, toEye);
    }
    
    [unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        color += ComputeSpotLight(light[i], material, input.posWorld, input.normalWorld, toEye);
    }

    // reflect(광선이 들어오는 방향, 노멀 벡터)
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-reflect
    
    // 여기까지하면 환경에서 들어온 빛만 반영 함.
    // return g_textureCube1.Sample(g_sampler, input.normalWorld);
    
    // 눈과 정반사되는 빛도 반영하자
    return g_textureCube1.Sample(g_sampler, reflect(-toEye, input.normalWorld));
    
    // return useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.texcoord) : float4(color, 1.0);
}

```