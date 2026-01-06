---
layout: default
title: "01. Prerequisite"
parent: "(Vulkan)"
grand_parent: "(Graphics 😎)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 내 gpu가 Vulkan 특정 버전을 지원하는지 확인

* [Gpu Info Vulkan](https://vulkan.gpuinfo.org/)

---

## Vulkan SKD Download

* [Vulkan SDK Down](https://vulkan.lunarg.com/)
    * SDK Installer로 받으면 됩니다
    * 구성요소 (SDL2 등) 모두 설치하는게 좋습니다

---

## visual studio vcpkg 지정

* 옵션 -> vcpkg 패키지 관리자 -> 일반 -> vcpkg.exe의 사용자 지정 경로 사용
* 프로젝트 속성 -> 구성 속성 -> vcpkg -> Use Vcpkg Manifest -> Yes
    * 만약 구성 속성에 vcpkg가 안보인다면 `vcpkg integrate install`를 해야함
* 여기까지 하면 프로젝트의 하위에 vcpkg.json이 있다면 빌드시 자동으로 vcpkg 빌드가 된다.


