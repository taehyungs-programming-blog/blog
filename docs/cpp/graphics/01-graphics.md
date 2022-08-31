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

## imgui 설치

```bash
$ git clone https://github.com/microsoft/vcpkg
$ cd .\vcpkg
$ .\bootstrap-vcpkg.bat
$ .\vcpkg search imgui
$ .\vcpkg install imgui[win32-binding,dx11-binding]:x64-windows
```

* 설치된 **imgui 라이브러리**를 프로젝트에 링킹시키면 됨.