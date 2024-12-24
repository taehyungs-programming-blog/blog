---
layout: default
title: "35. 시스템 로캘을 영어로 하고 UTF8로 인코딩된 한글을 브레이크 포인트를 걸어 볼 수 있을까?"
parent: (Windows API)
grand_parent: (C++)
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 시스템 로캘을 영어로 하고 UTF8로 인코딩된 한글을 브레이크 포인트를 걸어 볼 수 있을까?

* 정답 : No!
* Why? 
    * 시스템 로캘이 영어(CP437)의 경우 IDE에서 한글 UTF8를 변환을 못해 브레이크 포인트로 변수를 확인할 경우 깨져서 보인다.
    * 시스템 로캘이 한글(CP949)의 경우 IDE에서 한글 UTF8을 변환이 가능하기에 브레이크 포인트로 변수를 확인할 수 있다.
* 어떻게 하면 될까? 
    * 시스템 로캘의 `Beta: 세계 언어 지원을 위해 Unicode UTF-8 사용` 옵션을 키고 디버깅하면 된다.