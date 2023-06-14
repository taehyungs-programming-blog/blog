---
title: "webrtc : Example - 4 : Client내부 분석(연결시도 수신)"
permalink: webrtc/native-example-4/                # link 직접 지정
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-07-12 00:00:00 -0000
last_modified_at: 2020-07-12 00:00:00 -0000
sidebar:
  title: "webrtc 목차"
  nav: webrtc
tag:
  - webrtc
category:
  - example
  - peerconnection client
excerpt: ""
header:
  teaser: /file/image/webrtc-page-teaser.gif
---

## 우선은 빌드

우선 빈 프로젝트를 하나 생성

C/C++ -> 일반 -> 추가 포함 디렉터리

```
~\webrtc-code
~\webrtc-code\src
~\webrtc-code\src\third_party\abseil-cpp
```

링커 -> 일반 -> 추가 라이브러리 디렉터리

```
~\webrtc-code\src\out\vs2017-x64-debug\obj
```

링커 -> 입력 -> 추가 종속성

```
rtc_tools/command_line_parser.lib
advapi32.lib
comdlg32.lib
dbghelp.lib
dnsapi.lib
gdi32.lib
msimg32.lib
odbc32.lib
odbccp32.lib
oleaut32.lib
psapi.lib
shell32.lib
shlwapi.lib
user32.lib
usp10.lib
uuid.lib
version.lib
wininet.lib
winmm.lib
winspool.lib
ws2_32.lib
delayimp.lib
kernel32.lib
ole32.lib
```

C/C++ -> 전처리기

```
WIN32
_DEBUG
_WINDOWS
V8_DEPRECATION_WARNINGS
USE_AURA=1
NO_TCMALLOC
FULL_SAFE_BROWSING
SAFE_BROWSING_CSD
SAFE_BROWSING_DB_LOCAL
CHROMIUM_BUILD
FIELDTRIAL_TESTING_ENABLED
_HAS_EXCEPTIONS=0
__STD_C
_CRT_RAND_S
_CRT_SECURE_NO_DEPRECATE
_SCL_SECURE_NO_DEPRECATE
_ATL_NO_OPENGL
CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
PSAPI_VERSION=1
_SECURE_ATL
_USING_V110_SDK71_
WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP
WIN32_LEAN_AND_MEAN
NOMINMAX
_UNICODE
UNICODE
NTDDI_VERSION=0x0A000002
_WIN32_WINNT=0x0A00
WINVER=0x0A00
NDEBUG
NVALGRIND
DYNAMIC_ANNOTATIONS_ENABLED=0
WEBRTC_ENABLE_PROTOBUF=1
WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
HAVE_SCTP
USE_BUILTIN_SW_CODECS
WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=1
WEBRTC_WIN
ABSL_ALLOCATOR_NOTHROW=1
```

C/C++ -> 코드 생성 -> 런타임 라이브러리 -> 다중 스레드 디버그(/MTd)