---
layout: default
title: "1. Native WebRTC Build"
parent: "(native-WebRTC)"
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
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
$ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
$ cd depot_tools
:: 꼭 Clone으로 안받고 홈페이지에서 다운받아도 되는데 이게 편함.
```

* 환경변수
    * PATH에 depot_tools경로 추가
    * set DEPOT_TOOLS_WIN_TOOLCHAIN=0
    * set GYP_MSVS_VERSION=2022 -> 버전에 따라 변경
    * set GYP_GENERATORS=ninja,msvs-ninja
    * set GYP_DEFINES=component=shared_library target_arch=x64

```bat
:: bat을 만드는게 오히려 편할수도?
set PATH=PATH;
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GYP_MSVS_VERSION=2022
set GYP_GENERATORS=ninja,msvs-ninja
set GYP_DEFINES=component=shared_library target_arch=x64
```

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
$ gclient sync --force
:: depot_tools를 webrtc와 맞는 버전으로 checkout
```

### WebRTC Build

```bat
:: src 폴더내에서
$ gn gen --ide=vs out\Default

:: windows 10 64bit의 경우 warning LNK4075로 error LNK1218가 발생되어 빌드가 중지되는데, 
:: 이때에는 gn gen out/Default --args="fatal_linker_warnings=false" 로 빌드하면 된다.
```

```bat
:: 빌드
$ ninja -C out/Default  
:: 하여 Command Line상에서 Build 한다.
```

