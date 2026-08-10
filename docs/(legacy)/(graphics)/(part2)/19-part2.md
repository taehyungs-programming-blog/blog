---
layout: default
title: "19. 여러 Vertex위에 Texture 그리기(격자 평면 그리기)"
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

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/10_grid)

```cpp
MeshData GeometryGenerator::MakeGrid(const float width, const float height,
                                     const int numSlices, const int numStacks) {

    // 안내: 실린더와 구 만들기 이해를 돕기 위해 후속 녹화한 강의입니다.

    // x-y 평면 (z = 0) 위에 격자 구조로 평면 만들기
    // 뒤에서 복잡한 지형으로 확장

    // 1단계: numStacks = 1 이고 numSlices만 고려해서 구현
    // 2단계: 2차원 바둑판 구조

    const float dx = width / numSlices;
    const float dy = height / numSlices;

    MeshData meshData;

    vector<Vertex> &vertices = meshData.vertices;
    vector<uint16_t> &indices = meshData.indices;

    // y = -0.5f * height 인 점들
    //Vector3 stackStartPoint = Vector3(-0.5f * width, -0.5f * height, 0.0f);
    //for (int i = 0; i <= numSlices; i++) {
    //    Vertex v;

    //    // x-y 평면에서 시작점을 x 방향으로 이동
    //    v.position = Vector3::Transform(
    //        stackStartPoint,
    //        Matrix::CreateTranslation(Vector3(dx * float(i), 0.0f, 0.0f)));

    //    // 시점을 향하는 방향
    //    v.normal = Vector3(0.0f, 0.0f, -1.0f);
    //    v.texcoord = Vector2(float(i) / numSlices, 1.0f);

    //    vertices.push_back(v);
    //}

    Vector3 leftBottom = Vector3(-0.5f * width, -0.5f * height, 0.0f);
    for (int j = 0; j <= numStacks; j++) {
        // 스택은 위로 쌓기 때문에 y 방향으로 이동
        Vector3 stackStartPoint = Vector3::Transform(
            leftBottom, Matrix::CreateTranslation(Vector3(0.0f, dy * j, 0.0f)));

        for (int i = 0; i <= numSlices; i++) {
            Vertex v;

            // x-y 평면에서 시작점을 x 방향으로 이동
            v.position = Vector3::Transform(
                stackStartPoint,
                Matrix::CreateTranslation(Vector3(dx * float(i), 0.0f, 0.0f)));

            // 시점을 향하는 방향
            v.normal = Vector3(0.0f, 0.0f, -1.0f);
            v.texcoord = Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks);

            vertices.push_back(v);
        }
    }

    cout << vertices.size() << endl;

    //// y = 0.5f * height 인 점들
    //stackStartPoint = Vector3(-0.5f * width, 0.5f * height, 0.0f);
    //for (int i = 0; i <= numSlices; i++) {
    //    Vertex v;

    //    // x-y 평면에서 시작점을 x 방향으로 이동
    //    v.position = Vector3::Transform(
    //        stackStartPoint,
    //        Matrix::CreateTranslation(Vector3(dx * float(i), 0.0f, 0.0f)));

    //    // 시점을 향하는 방향
    //    v.normal = Vector3(0.0f, 0.0f, -1.0f);
    //    v.texcoord = Vector2(float(i) / numSlices, 1.0f);

    //    vertices.push_back(v);
    //}

    for (int j = 0; j < numStacks; j++) {

        const int offset = (numSlices + 1) * j;

        for (int i = 0; i < numSlices; i++) {

            // 첫번째 삼각형
            indices.push_back(offset + i);
            indices.push_back(offset + i + numSlices + 1);
            indices.push_back(offset + i + 1 + numSlices + 1);

            // 두 번째 삼각형
            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + numSlices + 1);
            indices.push_back(offset + i + 1);
        }
    }

    cout << indices.size() << endl;
    for (int i = 0; i < indices.size(); i++) {
        cout << indices[i] << " ";
    }
    cout << endl;

    return meshData;
}
```