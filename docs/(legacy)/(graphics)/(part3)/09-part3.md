---
layout: default
title: "09. Geometry Shader 활용 - Normal Vector 그리기"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/9/9.NormalVector)

* ??? 기존에  Normal Vector를 그리고 있었는데???
    * 기존부분을 빼고 Geometry Shader로 그리고자 한다
* 그럼 우선 기존에 어떻게 그려지고 있었느지 확인해 보자.

```cpp
class BasicMeshGroup {
    // ...

    // m_normalLines를 통해 그리고 있었음.
    shared_ptr<Mesh> m_normalLines;
```

```cpp
void BasicMeshGroup::Render(ComPtr<ID3D11DeviceContext> &context) {

    // ...

    // 노멀 벡터 그리기
    if (m_drawNormals) {
        context->VSSetShader(m_normalVertexShader.Get(), 0, 0);
        ID3D11Buffer *pptr[2] = {m_vertexConstantBuffer.Get(),
                                 m_normalVertexConstantBuffer.Get()};
        context->VSSetConstantBuffers(0, 2, pptr);
        context->PSSetShader(m_normalPixelShader.Get(), 0, 0);
        context->IASetInputLayout(m_basicInputLayout.Get());
        context->IASetVertexBuffers(
            0, 1, m_normalLines->vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(m_normalLines->indexBuffer.Get(),
                                  DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        context->DrawIndexed(m_normalLines->m_indexCount, 0, 0);
    }
}
```

* `m_normalLines`를 빼버리고 싶다는 말!

---

## 해보자!

* 새로운 NormalGeometryShader.hlsl 가 필요하다

```cpp
cbuffer BasicVertexConstantData : register(b0)
{
    matrix modelWorld;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

cbuffer NormalVertexConstantData : register(b1)
{
    float scale; // 그려지는 선분의 길이 조절
};

struct GeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalModel : NORMAL;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

[maxvertexcount(2)]
void main(point GeometryShaderInput input[1], inout LineStream<PixelShaderInput> outputStream)
{
    PixelShaderInput output;
    
    float4 posWorld = mul(input[0].posModel, modelWorld);
    float4 normalModel = float4(input[0].normalModel, 0.0);
    float4 normalWorld = mul(normalModel, invTranspose);
    normalWorld = float4(normalize(normalWorld.xyz), 0.0);
    
    output.pos = mul(posWorld, view);
    output.pos = mul(output.pos, projection);
    output.color = float3(1.0, 1.0, 0.0);
    outputStream.Append(output);
    
    output.pos = mul(posWorld + scale * normalWorld, view);
    output.pos = mul(output.pos, projection);
    output.color = float3(1.0, 0.0, 0.0);
    outputStream.Append(output);
}

```

```cpp
class BasicMeshGroup {
    // ...

    // Geometry Shader로 Normal Vector 그릴 준비
    ComPtr<ID3D11GeometryShader> m_normalGeometryShader;
```

```cpp
void BasicMeshGroup::Initialize(ComPtr<ID3D11Device> &device,
                                const std::vector<MeshData> &meshes) {

    // ...

    // Geometry shader 초기화하기
    D3D11Utils::CreateGeometryShader(device, L"NormalGeometryShader.hlsl",
                                     m_normalGeometryShader);
```

```cpp
void BasicMeshGroup::Render(ComPtr<ID3D11DeviceContext> &context) {
    //...

        if (m_drawNormals) {
            // 같은 VertexBuffer 사용
            context->VSSetShader(m_normalVertexShader.Get(), 0, 0);
            ID3D11Buffer *pptr[2] = {m_vertexConstantBuffer.Get(),
                                     m_normalVertexConstantBuffer.Get()};
            context->GSSetConstantBuffers(0, 2, pptr);
            context->GSSetShader(m_normalGeometryShader.Get(), 0, 0);
            context->PSSetShader(m_normalPixelShader.Get(), 0, 0);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
            context->Draw(mesh->m_vertexCount, 0);
            context->GSSetShader(nullptr, 0, 0);
        }
    }
}

```

{% endraw %}