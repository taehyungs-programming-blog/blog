---
layout: default
title: "11. D3D Init"
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

* [Clone Code 🌎](https://github.com/Arthur880708/Directx11_2_Rasterization/tree/main/1_InitD3D)

* 몰랐던 부분만 정리

## vcpkg 설치, directxtk 설치

```bash
# vcpkg 설치
$ git clone https://github.com/microsoft/vcpkg
$ cd .\vcpkg
$ .\bootstrap-vcpkg.bat
```

```bash
$ .\vcpkg install directxtk:x64-windows
$ .\vcpkg integrate install 
# vs에서 imgui를 사용하게 준비해 달라
```

---

## MSAA (MultiSample Anti-Aliasing)

* [참고사이트 🌎](https://mynameismjp.wordpress.com/2012/10/24/msaa-overview/)
* [참고사이트(MSDN) 🌎](https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules?redirectedfrom=MSDN)

```cpp
// 4X MSAA 지원하는지 확인
UINT numQualityLevels;
device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
if (numQualityLevels <= 0) {
    cout << "MSAA not supported." << endl;
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/directx11/d11-1-1.webp"/>
</p>

* Pixel의 색상을 결정 시 4개를 Sampling후 Anti Aliasing하는것이라 생각