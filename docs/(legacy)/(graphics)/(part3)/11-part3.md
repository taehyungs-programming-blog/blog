---
layout: default
title: "11. Tessellation"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/11/11.Tessellation)

* 우선 Tessellation이란 용어 자체는 하나의 이미지를 작은 sub이미지로 채우는 것을 의미한다

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-11-1.png"/>
</p>

* 하고자 하는것 ?
* 4개의 vertex를 shader내에서 여러개의 vertex로 해상도를 높여보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-11-2.png"/>
</p>

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-11-3.png"/>
</p>

* 이걸 어디다 쓰지?
* 멀리 있는 물체는 vertex해상도가 낮게 가까이 있는 물체는 vertex해상도를 높게 할때 쓰면 좋다(Level Of Detail(LOD))

* 해야할 일! -> 거리에 따라 vertex를 조절하는 object를 만들어 보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-11-4.png"/>
</p>

* Hull과 Tessellation은 한 쌍이기에 같이 만들어 줄 예정

---

```cpp
void TessellatedQuad::Initialize(ComPtr<ID3D11Device> &device) {

    // ...

    // Vetex Buffer 만들기
        // Vertex를 모은것을 controlPoint라 한다.
        // 그렇게 부르는 이유는 하나의 Vertex단위로 제어하는 것이 아니라
        // controlPoint 단위로 제어하기 때문!
    std::vector<Vector4> controlPoints = {{-1.0f, 1.0f, 0.0, 1.0f},
                                          {1.0f, 1.0f, 0.0, 1.0f},
                                          {-1.0f, -1.0f, 0.0, 1.0f},
                                          {1.0f, -1.0f, 0.0, 1.0f}};

    // ...
```

```cpp
cbuffer ConstantData : register(b0)
{
    float3 eyeWorld;
    float width;
    Matrix model;
    Matrix view;
    Matrix proj;
    float time = 0.0f;
    float3 padding;
    float4 edges;
    float2 inside;
    float2 padding2;
};

struct VertexOut
{
    float4 pos : POSITION;
};

struct HullOut
{
    float3 pos : POSITION;
};

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

PatchConstOutput MyPatchConstantFunc(InputPatch<VertexOut, 4> patch,
                                     uint patchID : SV_PrimitiveID)
{
    float3 center = (patch[0].pos + patch[1].pos + patch[2].pos + patch[3].pos).xyz * 0.25;
    float dist = length(center - eyeWorld);
    float distMin = 0.5;
    float distMax = 2.0;
    float tess = 64.0 * saturate((distMax - dist) / (distMax - distMin)) + 1.0;
    
    PatchConstOutput pt;
    
    pt.edges[0] = tess;
    pt.edges[1] = tess;
    pt.edges[2] = tess;
    pt.edges[3] = tess;
    pt.inside[0] = tess;
    pt.inside[1] = tess;
    return pt;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("MyPatchConstantFunc")]
[maxtessfactor(64.0f)]
HullOut main(InputPatch<VertexOut, 4> p,
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
    HullOut hout;
    hout.pos = p[i].pos.xyz;

    return hout;
}
```

---

- 흠... 흠?
- 사실 다 볼 것 없이 shader만 보면 되기에 shader만 설명하겠음

```cpp
// 우선 Rendering이 어떻게 돌아가나 보자

void TessellatedQuad::Render(ComPtr<ID3D11DeviceContext> &context) {

    // IA
    context->IASetInputLayout(m_inputLayout.Get());
    UINT stride = sizeof(Vector4); // sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride,
                                &offset);

    // VerTex Shader
    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Pixel Shader
    context->PSSetShader(m_pixelShader.Get(), 0, 0);
    context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());


    //ID3D11ShaderResourceView *resViews[3] = {m_texArraySRV.Get(),
    //                                         m_diffuseResView.Get(),
    //                                         m_specularResView.Get()};
    //context->PSSetShaderResources(0, 3, resViews);

    // Hull shader
    context->HSSetShader(m_hullShader.Get(), 0, 0);
    context->HSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Domain shader
    context->DSSetShader(m_domainShader.Get(), 0, 0);
    context->DSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    context->DSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // 토폴로지를 4개의 Control Point로 설정
        // 무슨말인고 하면, 4개의 Vertex를 사용하는 Control Point를 그려달라
    context->IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
    context->Draw(m_indexCount, 0);

    // HS, DS를 사용하지 않는 다른 물체들을 위해 nullptr로 설정
    context->HSSetShader(nullptr, 0, 0);
    context->DSSetShader(nullptr, 0, 0);
}
```

* Vertex -> Hull Shader

```cpp
cbuffer ConstantData : register(b0)
{
    float3 eyeWorld;
    float width;
    Matrix model;
    Matrix view;
    Matrix proj;
    float time = 0.0f;
    float3 padding;
    float4 edges;       // 가장자리
    float2 inside;      // x, y 방향으로 쪼갬
    float2 padding2;
};

struct VertexOut
{
    float4 pos : POSITION;
};

struct HullOut
{
    float3 pos : POSITION;
};

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

PatchConstOutput MyPatchConstantFunc(InputPatch<VertexOut, 4> patch,
                                     uint patchID : SV_PrimitiveID)
{
    float3 center = (patch[0].pos + patch[1].pos + patch[2].pos + patch[3].pos).xyz * 0.25;
    float dist = length(center - eyeWorld);
    float distMin = 0.5;
    float distMax = 2.0;
    float tess = 64.0 * saturate((distMax - dist) / (distMax - distMin)) + 1.0;
    
    PatchConstOutput pt;
    
    pt.edges[0] = tess;
    pt.edges[1] = tess;
    pt.edges[2] = tess;
    pt.edges[3] = tess;
    pt.inside[0] = tess;
    pt.inside[1] = tess;
    return pt;
}

[domain("quad")]                    // 사격형을 의미
[partitioning("integer")]           // 어떻게 쪼갤지
[outputtopology("triangle_cw")]     // 분할후 시계방향 삼각형으로 나눈다
[outputcontrolpoints(4)]            // 컨트롤 포인트가 몇개인지
[patchconstantfunc("MyPatchConstantFunc")]  // constantc func 이름
[maxtessfactor(64.0f)]
HullOut main(InputPatch<VertexOut, 4> p,
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
    HullOut hout;
    hout.pos = p[i].pos.xyz;

    return hout;
}

```

```cpp
cbuffer ConstantData : register(b0)
{
    float3 eyeWorld;
    float width;
    Matrix model;
    Matrix view;
    Matrix proj;
    float time = 0.0f;
    float3 padding;
};

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct HullOut
{
    float3 pos : POSITION;
};

struct DomainOut
{
    float4 pos : SV_POSITION;
};

[domain("quad")]
DomainOut main(PatchConstOutput patchConst,
             float2 uv : SV_DomainLocation,
             const OutputPatch<HullOut, 4> quad)
{
    DomainOut dout;

	// Bilinear interpolation.
    float3 v1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 v2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(v1, v2, uv.y);
    
    dout.pos = float4(p, 1.0);
    dout.pos = mul(dout.pos, view);
    dout.pos = mul(dout.pos, proj);
    return dout;
}

```

{% endraw %}