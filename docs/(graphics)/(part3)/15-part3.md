---
layout: default
title: "15. HDRI(High Dynamic Range Images)"
parent: "(Rendering Techniques)"
grand_parent: "(Graphics 😎)"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [Get Code 🌎](https://github.com/Arthur880708/Graphics_Part3_Examples/tree/15/15.HDRI)

* 하고싶은건 뭐냐?
* Color 해상도가 높은 이미지를 가져와 Texture로 사용하고자 한다
* `DXGI_FORMAT_R16G16B16A16_FLOAT` 정도를 쓰고싶음

---

* hdr파일을 불러오기 위해서는 `DirectXTex`를 사용해야 한다.
* `$ vcpkg install directxtex[openexr]:x64-windows`로 설치
* 추가적으로 float이 16bit 인데 32bit float을 쓰기위해서 `$ vcpkg install fp16:x64-windows` 설치

---

* (참고) HDR 큐브 맵 만들기
    * https://github.com/derkreature/IBLBaker 에서 코드 다운
    * bin64에서 IBLBaker.exe 실행

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/graphics/part3/p3-15-1.png"/>
</p>

* Load Environment를 눌러 HDR파일을 연다
* Save Environment를 눌러 저장, 단, 확장자 dds를 꼭 기입해 줘야함.

{% endraw %}