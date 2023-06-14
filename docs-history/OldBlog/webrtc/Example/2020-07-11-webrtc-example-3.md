---
title: "webrtc : Example - 3 : PeerConnection Client 측"
permalink: webrtc/native-example-3/                # link 직접 지정
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

Windows 데스크톱 애플리케이션 하나 생성

![](/file/image/natvie-example-3-1.png)

C/C++ -> 일반 -> 추가 포함 디렉터리

```
~\webrtc-code
~\webrtc-code\src
~\webrtc-code\src\third_party\abseil-cpp
~\webrtc-code\src\third_party\libyuv\include
```

링커 -> 일반 -> 추가 라이브러리 디렉터리

```
~\webrtc-code\src\out\vs2017-x64-debug\obj
```

링커 -> 입력 -> 추가 종속성

```
\third_party\libjpeg_turbo\libjpeg.lib
\third_party\libjpeg_turbo\simd.lib
\third_party\libjpeg_turbo\simd_asm.lib
\third_party\libyuv\libyuv_internal.lib
\third_party\boringssl\boringssl.lib
\third_party\boringssl\boringssl_asm.lib
\third_party\libsrtp\libsrtp.lib
\third_party\libvpx\libvpx.lib
\third_party\libvpx\libvpx_yasm.lib
\third_party\opus\opus.lib
\third_party\protobuf\protobuf_full.lib
\third_party\protobuf\protobuf_lite.lib
\third_party\protobuf\protoc_lib.lib
\third_party\usrsctp\usrsctp.lib
\third_party\winsdk_samples\winsdk_samples.lib
\third_party\yasm\yasm_utils.lib
\api\audio_codecs\builtin_audio_decoder_factory.lib
\api\libjingle_peerconnection_api.lib
\audio\audio.lib
\audio\utility\audio_frame_operations.lib
\rtc_base\rtc_base.lib
\rtc_base\rtc_numerics.lib
\call\call.lib
\common_audio\common_audio.lib
\common_audio\common_audio_sse2.lib
\common_video\common_video.lib
\logging\rtc_event_log_proto.lib
\media\rtc_media_base.lib
\modules\audio_coding\ana_config_proto.lib
\modules\audio_coding\ana_debug_dump_proto.lib
\modules\audio_coding\audio_coding.lib
\modules\audio_coding\audio_format_conversion.lib
\modules\audio_coding\audio_network_adaptor.lib
\modules\audio_coding\cng.lib
\modules\audio_coding\g711.lib
\modules\audio_coding\g722.lib
\modules\audio_coding\ilbc.lib
\modules\audio_coding\isac.lib
\modules\audio_coding\isac_c.lib
\modules\audio_coding\isac_common.lib
\modules\audio_coding\legacy_encoded_audio_frame.lib
\modules\audio_coding\neteq.lib
\modules\audio_coding\pcm16b.lib
\modules\audio_coding\red.lib
\modules\audio_coding\rent_a_codec.lib
\modules\audio_coding\webrtc_opus.lib
\modules\audio_mixer\audio_frame_manipulator.lib
\modules\audio_mixer\audio_mixer_impl.lib
\modules\audio_processing\audioproc_debug_proto.lib
\modules\audio_processing\audio_processing.lib
\modules\bitrate_controller\bitrate_controller.lib
\modules\congestion_controller\congestion_controller.lib
\modules\desktop_capture\desktop_capture_differ_sse2.lib
\modules\desktop_capture\primitives.lib
\modules\pacing\pacing.lib
\modules\remote_bitrate_estimator\remote_bitrate_estimator.lib
\modules\rtp_rtcp\rtp_rtcp.lib
\modules\utility\utility.lib
\modules\video_capture\video_capture_module.lib
\modules\video_coding\video_coding.lib
\modules\video_coding\webrtc_h264.lib
\modules\video_coding\webrtc_vp8.lib
\modules\video_coding\webrtc_vp9.lib
\modules\video_processing\video_processing.lib
\modules\video_processing\video_processing_sse2.lib
\ortc\ortc.lib
\p2p\libstunprober.lib
\p2p\rtc_p2p.lib
\stats\rtc_stats.lib
\system_wrappers\system_wrappers.lib
\video\video.lib
webrtc.lib
winmm.lib
dmoguids.lib
wmcodecdspuuid.lib
amstrmid.lib
msdmo.lib
crypt32.lib
iphlpapi.lib
secur32.lib
Strmiids.lib
ws2_32.lib
```

C/C++ -> 전처리기

```
_DEBUG
_CONSOLE
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
_WINDOWS
CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
PSAPI_VERSION=1
WIN32
_SECURE_ATL
USING_V110_SDK71
WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP
WIN32_LEAN_AND_MEAN
NOMINMAX
_UNICODE
UNICODE
NTDDI_VERSION=0x0A000002
_WIN32_WINNT=0x0A00
WINVER=0x0A00
NVALGRIND
DYNAMIC_ANNOTATIONS_ENABLED=0
WEBRTC_ENABLE_PROTOBUF=1
WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
HAVE_SCTP
USE_BUILTIN_SW_CODECS
WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=1
WEBRTC_WIN
ABSL_ALLOCATOR_NOTHROW=1
HAVE_WEBRTC_VIDEO
HAVE_WEBRTC_VOICE
```

C/C++ -> 코드 생성 -> 런타임 라이브러리 -> 다중 스레드 디버그(/MTd)

### 코드 삽입

요렇게 넣으면 됨.

![](/file/image/natvie-example-3-2.png)

---