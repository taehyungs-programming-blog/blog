---
layout: default
title: "24. Cube Mapping"
parent: "(Realtime Pipeline)"
grand_parent: "(Graphics 😎)"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/16_cubeMapping)

* Cube Mapping으로 배경을 그려보자

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-14-1.jpg"/>
</p>

---

## Cube Mapping .dds 파일 만들기

```bash
$ texassemble.exe cube -w 2048 -h 2048 -o saintpeters.dds posx.jpg negx.jpg posy.jpg negy.jpg posz.jpg negz.jpg 
$ texassemble.exe cube -w 2048 -h 2048 -o skybox.dds right.jpg left.jpg top.jpg bottom.jpg front.jpg back.jpg -y
# https://github.com/Microsoft/DirectXTex/wiki/Texassemble 에서 texassemble 다운가능.
```

---

## DDS 읽고 Texture로 쓰기

```cpp
void ExampleApp::InitializeCubeMapping() {
    // .dds 파일 읽어들여서 초기화
    ComPtr<ID3D11Texture2D> texture;
    auto hr = CreateDDSTextureFromFileEx(
        // this->m_device.Get(), L"./SaintPetersBasilica/saintpeters.dds", 0,
        this->m_device.Get(), L"./skybox/skybox.dds", 0, D3D11_USAGE_DEFAULT,
        D3D11_BIND_SHADER_RESOURCE, 0,
        D3D11_RESOURCE_MISC_TEXTURECUBE, // 큐브맵용 텍스춰
        DDS_LOADER_FLAGS(false), (ID3D11Resource **)texture.GetAddressOf(),
        this->m_cubeMapping.cubemapResourceView.GetAddressOf(), nullptr);

    // ...

    // 커다란 박스 초기화
    // - 세상이 커다란 박스 안에 갇혀 있는 구조입니다.
    // - D3D11_CULL_MODE::D3D11_CULL_NONE 또는 삼각형 뒤집기
    // - 예시) std::reverse(myvector.begin(),myvector.end());
    MeshData cubeMeshData = GeometryGenerator::MakeBox(20.0f);
    std::reverse(cubeMeshData.indices.begin(), cubeMeshData.indices.end());

    AppBase::CreateVertexBuffer(cubeMeshData.vertices,
                                m_cubeMapping.cubeMesh->vertexBuffer);
    m_cubeMapping.cubeMesh->m_indexCount = UINT(cubeMeshData.indices.size());
    AppBase::CreateIndexBuffer(cubeMeshData.indices,
                               m_cubeMapping.cubeMesh->indexBuffer);
```