---
layout: default
title: "18. Vertex의 Normal Vector 표시"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/9_drawingnormal)

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-8-1.png"/>
</p>

```cpp
#include "Common.hlsli"

cbuffer NormalVertexConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
    float scale; // 그려지는 선분의 길이 조절
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.posModel, 1.0f);

    // Normal 먼저 변환
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    pos = mul(pos, model);
    
    float t = input.texcoord.x;
    
    // 시작점의 경우 texcoord이 0
    // 끝점의 경우 texcoord가 1
    // output.normalWorld가 normal Vector이기에 더한만큼 위치가 옮겨지겠지?
    pos.xyz += output.normalWorld * t * scale;

    output.posWorld = pos.xyz;
    
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.posProj = pos;
    output.texcoord = input.texcoord;
    
    output.color = float3(1.0, 1.0, 0.0) * (1.0 - t) + float3(1.0, 0.0, 0.0) * t;

    return output;
}
```
