---
layout: default
title: "20. 실린더 그리기"
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

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/11_cynlinder)


* 주의할 점은 normal vector를 계산할때이다.

```cpp
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.posModel, 1.0f);

    // Normal 먼저 변환
    float4 normal = float4(input.normalModel, 0.0f);

    // inv transpose를 곱해줘야함
    output.normalWorld = mul(normal, invTranspose).xyz;

    // 그냥 model view를 곱하면 안된다.
    //output.normalWorld = mul(normal, model).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    pos = mul(pos, model);
    
    float t = input.texcoord.x;
    
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

```cpp
MeshData GeometryGenerator::MakeCylinder(const float bottomRadius,
                                         const float topRadius, float height,
                                         int sliceCount) {

    using namespace DirectX;
    using DirectX::SimpleMath::Matrix;
    using DirectX::SimpleMath::Vector3;

    // Texture 좌표계때문에 (sliceCount + 1) x 2 개의 버텍스 사용

    const float dTheta = -XM_2PI / float(sliceCount);

    MeshData meshData;

    vector<Vertex> &vertices = meshData.vertices;

    // 옆면의 바닥 버텍스들 (인덱스 0 이상 sliceCount 미만)
    for (int i = 0; i <= sliceCount; i++) {
        Vertex v;

        // TODO: 작성 (텍스춰 좌표계, 버텍스 노멀 필요)
        v.position =
            Vector3::Transform(Vector3(bottomRadius, -0.5f * height, 0.0f),
                               Matrix::CreateRotationY(dTheta * float(i)));

        std::cout << v.position.x << " " << v.position.z << std::endl;

        v.normal = v.position - Vector3(0.0f, -0.5f * height, 0.0f);
        v.normal.Normalize();
        v.texcoord = Vector2(float(i) / sliceCount, 1.0f);

        vertices.push_back(v);
    }

    // 옆면의 맨 위 버텍스들 (인덱스 sliceCount 이상 2 * sliceCount 미만)
    for (int i = 0; i <= sliceCount; i++) {
        Vertex v;
        
        // TODO: 작성 (텍스춰 좌표계, 버텍스 노멀 필요)
        v.position =
            Vector3::Transform(Vector3(topRadius, 0.5f * height, 0.0f),
                               Matrix::CreateRotationY(dTheta * float(i)));

        v.normal = v.position - Vector3(0.0f, 0.5f * height, 0.0f);
        v.normal.Normalize();
        v.texcoord = Vector2(float(i) / sliceCount, 0.0f);

        vertices.push_back(v);
    }

    vector<uint16_t> &indices = meshData.indices;

    for (int i = 0; i < sliceCount; i++) {
        // TODO: 삼각형 두 개 씩
        indices.push_back(i);
        indices.push_back(i + sliceCount + 1);
        indices.push_back(i + 1 + sliceCount +1);

        indices.push_back(i);
        indices.push_back(i+1+sliceCount+1);
        indices.push_back(i+1);
    }

    return meshData;
}
```