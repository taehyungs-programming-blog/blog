---
layout: default
title: "1. Build and Test"
parent: "(native-WebRTC)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## build

* [WebRTC Home 🌎](https://webrtc.github.io/webrtc-org/native-code/development/)

### Prerequisite Software

* [참고 🌎](https://webrtc.github.io/webrtc-org/native-code/development/prerequisite-sw/)

#### depot_tools

```bat
$ cd depot_tools
$ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
:: 사용하려는 버전에 맞게 git checkout을 해줘야함.
:: 일단 최신버전을 사용한다 가정하자.
$ gclient
:: gclient - depot_tools의 checkout 도구(관련 메타파일을 모두 checkout하는듯)
```
* 환경변수
    * PATH에 depot_tools경로 추가
    * set DEPOT_TOOLS_WIN_TOOLCHAIN=0
    * set GYP_MSVS_VERSION=2022 -> 버전에 따라 변경
    * set GYP_GENERATORS=ninja,msvs-ninja
    * set GYP_DEFINES=component=shared_library target_arch=x64

### WebRTC Clone

```bat
$ mkdir webrtc-checkout
$ cd webrtc-checkout
$ fetch --nohooks webrtc
:: webrtc를 clone받고 단, history는 받지 않음
:: (오래걸림.)

$ cd src
$ git branch -r
:: 사용할 버전으로 checkout해주자
:: Ex) git checkout branch-heads/70
$ gclient sync 
:: 필요에 따라 --force 옵션 추가
:: 역시 관련 데이터 checkout해줘야함.
```

### WebRTC Build

```bat
:: src 폴더내에서
$ gn gen --ide=vs out\Default
```

---

## Test


