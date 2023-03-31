---
layout: default
title: "07. 기하 쉐이더(Geometry Shader))"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Get Code 🌎]()

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-7-1.jpg"/>
</p>

* Pixel Shader 이후 Hull, Tessellation, Domain이 있지만 지금은 생략하고 Geometry Shader에 대해서 알아보자.

---

## 구현

```cpp
[maxvertexcount(4)] // 최대 출력 Vertex 갯수
void main(point GeometryShaderInput input[1], uint primID : SV_PrimitiveID,
                              // inout PointStream<PixelShaderInput> outputStream)
                            inout TriangleStream<PixelShaderInput> outputStream)
{
    
    // float hw = 0.5 * width;
    
    PixelShaderInput output;
    
    float hw = 0.5 * width;
    
    output.pos = input[0].pos + float4(-hw, -hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(-hw, hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(hw, -hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos = input[0].pos + float4(hw, hw, 0.0f, 0.0f);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.primID = primID;
    
    outputStream.Append(output);
```