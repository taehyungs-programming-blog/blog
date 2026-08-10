---
layout: default
title: "21. 구 그리기"
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

## 일반적 방법

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/12_sphere)

```cpp
MeshData GeometryGenerator::MakeSphere(const float radius, const int numSlices,
                                       const int numStacks) {

    // 참고: OpenGL Sphere
    // http://www.songho.ca/opengl/gl_sphere.html
    // Texture 좌표계때문에 (numSlices + 1) 개의 버텍스 사용 (마지막에 닫아주는
    // 버텍스가 중복) Stack은 y 위쪽 방향으로 쌓아가는 방식

    const float dTheta = -XM_2PI / float(numSlices);
    const float dPhi = -XM_PI / float(numStacks);

    MeshData meshData;

    vector<Vertex> &vertices = meshData.vertices;

    for (int j = 0; j <= numStacks; j++) {

        // 스택에 쌓일 수록 시작점을 x-y 평면에서 회전 시켜서 위로 올리는 구조
        //Vector3 stackStartPoint = Vector3(
        //    radius, -radius + 2.0f * radius / numStacks * j, 0.0f); // 실린더
        Vector3 stackStartPoint = Vector3::Transform(
            Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * j));

        for (int i = 0; i <= numSlices; i++) {
            Vertex v;

            // 시작점을 x-z 평면에서 회전시키면서 원을 만드는 구조
            v.position = Vector3::Transform(
                stackStartPoint, Matrix::CreateRotationY(dTheta * float(i)));

            v.normal = v.position; // 원점이 구의 중심
            v.normal.Normalize();
            v.texcoord =
                Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks);

            vertices.push_back(v);
        }
    }

    // cout << vertices.size() << endl;

    vector<uint16_t> &indices = meshData.indices;

    for (int j = 0; j < numStacks; j++) {

        const int offset = (numSlices + 1) * j;

        for (int i = 0; i < numSlices; i++) {

            indices.push_back(offset + i);
            indices.push_back(offset + i + numSlices + 1);
            indices.push_back(offset + i + 1 + numSlices + 1);

            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + numSlices + 1);
            indices.push_back(offset + i + 1);
        }
    }

    // cout << indices.size() << endl;
    // for (int i = 0; i < indices.size(); i++) {
    //     cout << indices[i] << " ";
    // }
    // cout << endl;

    return meshData;
}
```

---

## 다른 방법 (Subdivision)

* [Clone Code 🌎](https://github.com/EasyCoding-7/DirectX11-Examples/tree/13/13_subdivision)

* 말 그대로 division을 높이는 방법이다.
* Vertex의 상세도가 낮을경우 사용하면 좋다.

```cpp
MeshData GeometryGenerator::SubdivideToSphere(const float radius,
                                              MeshData meshData) {

    using namespace DirectX;
    using DirectX::SimpleMath::Matrix;
    using DirectX::SimpleMath::Vector3;

    // 원점이 중심이라고 가정
    // 입력 받은 구 모델의 반지름 조절
    for (auto &v : meshData.vertices) {
        v.position = v.normal * radius;
    }

    // 구의 표면으로 옮기고 노멀 계산
    auto ProjectVertex = [&](Vertex &v) {
        v.normal = v.position;
        v.normal.Normalize();
        v.position = v.normal * radius;

        // 주의: 텍스춰가 이음매에서 깨집니다.
        // atan vs atan2
        // https://stackoverflow.com/questions/283406/what-is-the-difference-between-atan-and-atan2-in-c
        // const float theta = atan2f(v.position.z, v.position.x);
        // const float phi = acosf(v.position.y / radius);
        // v.texcoord.x = theta / XM_2PI;
        // v.texcoord.y = phi / XM_PI;
    };

    // 버텍스가 중복되는 구조로 구현
    MeshData newMesh;
    uint16_t count = 0;
    for (size_t i = 0; i < meshData.indices.size(); i += 3) {
        size_t i0 = meshData.indices[i];
        size_t i1 = meshData.indices[i + 1];
        size_t i2 = meshData.indices[i + 2];

        Vertex v0 = meshData.vertices[i0];
        Vertex v1 = meshData.vertices[i1];
        Vertex v2 = meshData.vertices[i2];

        Vertex v3;
        // 위치와 텍스춰 좌표 결정
        v3.position = (v0.position + v2.position) * 0.5f;
        v3.texcoord = (v0.texcoord + v2.texcoord) * 0.5f;

        Vertex v4;
        // 위치와 텍스춰 좌표 결정
        v4.position = (v0.position + v1.position) * 0.5f;
        v4.texcoord = (v0.texcoord + v1.texcoord) * 0.5f;

        Vertex v5;
        // 위치와 텍스춰 좌표 결정
        v5.position = (v1.position + v2.position) * 0.5f;
        v5.texcoord = (v1.texcoord + v2.texcoord) * 0.5f;

        ProjectVertex(v3);
        ProjectVertex(v4);
        ProjectVertex(v5);

        // 모든 버텍스 새로 추가
        newMesh.vertices.push_back(v4);
        newMesh.vertices.push_back(v1);
        newMesh.vertices.push_back(v5);

        newMesh.vertices.push_back(v0);
        newMesh.vertices.push_back(v4);
        newMesh.vertices.push_back(v3);

        newMesh.vertices.push_back(v3);
        newMesh.vertices.push_back(v4);
        newMesh.vertices.push_back(v5);

        newMesh.vertices.push_back(v3);
        newMesh.vertices.push_back(v5);
        newMesh.vertices.push_back(v2);

        // 인덱스 업데이트
         for (uint16_t j = 0; j < 12; j++) {
             newMesh.indices.push_back(j + count);
         }
         count += 12;
    }

    return newMesh;
}
} // namespace hlab

```