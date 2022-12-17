---
layout: default
title: "1. 환경설정"
parent: "(Graphics🖼)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 환경

* Visual Studio 2022
* DirectX 11

---

## vcpkg를 이용한 imgui 설치

```bash
$ git clone https://github.com/microsoft/vcpkg
$ cd .\vcpkg
$ .\bootstrap-vcpkg.bat
# pc에 vcpkg설치
$ .\vcpkg search imgui
# 설치가능(다운가능)한 imgui관련 패키지를 보여달라
$ .\vcpkg install imgui[win32-binding,dx11-binding]:x64-windows
# imgui를 win32, dx11용으로 다운받아달라
$ .\vcpkg integrate install 
# vs에서 imgui를 사용하게 준비해 달라
```

* 설치된 **imgui 라이브러리**를 프로젝트에 링킹시키면 됨.

---

## Example Project 살펴보기

* [Clone Code 🌎](https://github.com/EasyCoding-7/Dx11ExampleWithImgui/)

* 실행해 보면 아래와 같이 뜬다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/graphics/graphics-1-2.png"/>
</p>

* 아직은 모두 다 살펴볼 필요는 없고 대략

```cpp
void Update()
{
    // 뭔가 Update를 지나며 여기서 색상을 넣어준다 정도로 이해하자. 
    std::vector<Vec4> pixels(canvasWidth * canvasHeight, Vec4{0.8f, 0.8f, 0.8f, 1.0f});
        // std::vector<Vec4> pixels(canvasWidth * canvasHeight, Vec4{1.0f, 0.0f, 0.0f, 1.0f});
        // 이렇게 할 경우 배경이 빨강이 된다.
    pixels[0 + canvasWidth * 0] = Vec4{ 1.0f, 0.0f, 0.0f, 1.0f };
    pixels[1 + canvasWidth * 0] = Vec4{ 1.0f, 1.0f, 0.0f, 1.0f };

    // Update texture buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    deviceContext->Map(canvasTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, pixels.data(), pixels.size() * sizeof(Vec4));
    deviceContext->Unmap(canvasTexture, NULL);
}
```

* 말하고자 하는것은 DirectX(3D) 영역도 결국 Pixel의 색상을 변경함으로서 그려진다는 것이다.!
* 너무 어렵게만 생각하지말자.