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