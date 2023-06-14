---
title: "(WebRTC) Native Build"
permalink: webrtc/native_build/ # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-22 00:00:00 -0000
last_modified_at: 2020-06-01 00:00:00 -0000
sidebar:
  title: "WebRTC 목차"
  nav: webrtc
---

## 사전사항

### python 설치

우선 python 2.7.x가 설치되어 있어야한다.

![](/file/image/webrtc-native-build-image-03.png)

설치시 path지정 잊지말기

```s
# python 업데이트
$ python -m pip install pywin32
```

### Windows10 SDK 설치

VS Installer에서 설치되는거 보다 홈페이지에서 다운 받는걸 추천<br>
경로의 차이가 있다.

## depot_tools 설치

```s
$ mkdir depot_tools
$ cd depot_tools
$ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
$ gclient # webrtc 버전관리도구 설치
```

WebRTC 70버전과 depot_tools의 버전을 맞춰야 한다.

```s
$ git checkout c68a1753c53a02b1bd292035e4a72a18e55814f3
```

```
커밋: c68a1753c53a02b1bd292035e4a72a18e55814f3 [c68a175]
상위 항목: 38d36a616d
작성자: Andrii Shyshkalov <tandrii@chromium.org>
날짜: 2018년 10월 16일 화요일 오전 6:55:41
커밋한 사람: Commit Bot
git cl try-results: delete Rietveld support.

R=ehmaldonado

Bug: 770408
Change-Id: I94ff37f198937dc7d341a859a71becd91cf3a706
Reviewed-on: https://chromium-review.googlesource.com/c/1279140
Commit-Queue: Andrii Shyshkalov <tandrii@chromium.org>
Reviewed-by: Edward Lesmes <ehmaldonado@chromium.org>
```

![](/file/image/webrtc-native-build-image-01.png)

---

## 환경변수 설정

`PATH = ~webrtc\depot_tools\depot_tools`

gclient가 있는 폴더를 PATH에 넣으면 된다.

`DEPOT_TOOLS_WIN_TOOLCHAIN = 0`

---

## webrtc 다운로드

```s
$ mkdir webrtc-checkout
$ cd webrtc-checkout
# fetch --nohooks: git의 history는 복사하지 말아달라
# gclient는 checkout도구이고 fetch는 일종의 wrapper라 생각하자
$ fetch --nohooks webrtc
$ cd src
# branch -r : 원격 저장소의 브랜치를 보여달라
$ git branch -r
# 최신 버전을 선택
$ git checkout branch-heads/?
$ gclient sync --force    # 이거 꼭 해야함.
```

> 70이전 버전 빌드를 위해서는 gclient를 예전버전으로 체크아웃해야한다.

![](/file/image/webrtc-native-build-image-02.png)

---

## 빌드

```s
# 환경변수 선언
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
set GYP_MSVS_VERSION=2017
set GYP_GENERATORS=ninja,msvs-ninja
set GYP_DEFINES=component=shared_library target_arch=x64

# in src folder
$ gn gen out/x64/Debug --args="is_debug=true use_rtti=true target_cpu=\"x64\""

# .sln 파일생성
$ gn gen --ide=vs out\Default

# visual studio 자체 빌드가 되지 않기에 빌드는 위에 처럼하거나 아래처럼 간단빌드가능
$ gn gen out/Default --args="fatal_linker_warnings=false is_debug=false"
$ ninja -C out/Default
```

bat로 작성하기

```
:: m74이후 버전은 is_clang=true로 둬야 정상동작한다...

:: x64 Debug
call gn clean out/vs2017-x64-debug
call gn gen out/vs2017-x64-debug --winsdk="10.0.18362" "--args=is_debug=true is_clang=false symbol_level=2 rtc_use_h264=true target_cpu=\"x64\""
call ninja -C out/vs2017-x64-debug webrtc

:: x64 Release
call gn clean out/vs2017-x64-release
call gn gen out/vs2017-x64-release --winsdk="10.0.18362" "--args=is_debug=false is_clang=false rtc_use_h264=true target_cpu=\"x64\""
call ninja -C out/vs2017-x64-release webrtc

:: .sln 생성(단 VS에서 빌드는 안됨)
$ gn gen --ide=vs out\VisualStuio_sln
```

---

* [참고사이트1](https://sourcey.com/articles/building-and-installing-webrtc-on-windows)
* [참고사이트2](https://alnova2.tistory.com/1114)