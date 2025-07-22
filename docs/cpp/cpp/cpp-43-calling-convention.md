---
layout: default
title: "43. calling convention (__stdcall 등)"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 7
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## calling convention 정리

| 호출 규약        | 인자 순서       | 스택 정리 주체 | 비고                |
| ------------ | ----------- | -------- | ----------------- |
| `__cdecl`    | 오른쪽 → 왼쪽    | 호출자      | C의 기본 규약          |
| `__stdcall`  | 오른쪽 → 왼쪽    | 함수       | WinAPI, COM       |
| `__fastcall` | 일부 인자는 레지스터 | 함수       | 고성능용              |
| `__thiscall` | 클래스 멤버함수 전용 | 함수       | `this`는 `ecx`로 전달 |

---

## __stdcall은

* `__stdcall`은 Windows 플랫폼에서 사용되는 **함수 호출 규약(calling convention)** 중 하나로, 함수의 인자 전달 방식과 호출 후 스택 정리에 대한 규칙을 정의
* 함수 인자를 오른쪽에서 왼쪽 순서로 스택에 푸시(push)하고, 함수 자체가 호출 이후 스택을 정리한다는 특징

### 예를들어보자

```cpp
// __cdecl: 호출자가 스택을 정리
int __cdecl Add(int a, int b);

// __stdcall: 함수가 스택을 정리
int __stdcall Add(int a, int b);
```

```cpp
// 컴파일 후 심볼
// __cdecl: Add
// __stdcall: _Add@8
```

### 왜 쓸까?

* 호환성: Windows API 대부분이 __stdcall을 사용
* 스택 정리 안정성: 함수가 스택을 정리하기 때문에 호출자가 일관되게 호출할 수 있어, 다수의 호출자에게 공통된 인터페이스를 제공하는 DLL 또는 API에 적합
* COM 인터페이스 규약: COM은 기본적으로 __stdcall을 사용하도록 설계

---

## 64bit에서 calling convention?

* 32비트 환경에서 스레드 함수는 __stdcall 호출 규약을 강제
    * Win32 API 설계와 스택 정리의 안전성 보장 때문
    * API 내부에서 호출 후 스택을 정리하는 방식으로 구현

* 그럼 64bit는?
    * 64비트 Windows에서는 모든 함수 호출이 통일된 규약 (Windows x64 calling convention) 을 사용
    * 첫 4개의 인자를 레지스터로 전달하고, 호출자가 스택을 정리
    * 64비트에서는 호출 규약이 단일화되어 있어 __stdcall / __cdecl 같은 구분이 무의미

