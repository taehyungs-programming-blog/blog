---
layout: default
title: "46. llvm 이란?"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 7
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 도입) 컴파일 과정의 문제점

* C++ 코드를 실행 파일로 만들려면 다음 작업이 필요합니다:
    * 소스 코드 분석 (문법 검사, 구조 파악)
    * 코드 최적화 (더 빠르게 실행되도록 변환)
    * 기계어 생성 (CPU가 이해하는 명령어로 변환)
* **문제:** CPU마다 기계어가 다릅니다.
    * Intel/AMD CPU → x86-64 기계어
    * Apple M1/M2 → ARM64 기계어
    * 라즈베리파이 → ARM 기계어
* 그래서 컴파일러 개발자는 각 **CPU마다 기계어 생성 코드를 따로 작성**

## 컴파일러 개발자가 매번 CPU 기계어 생성문제를 해결해보자 해서 나온게 LLVM

* LLVM이 해결하는 방법
* LLVM은 중간 언어(IR: Intermediate Representation)를 도입

### 실제 예시

* 이런 C++ 코드가 있다고 가정

```cpp
int add(int a, int b) {    
    return a + b;
}
```

* 이 코드는 LLVM IR로 변환

```cpp
define i32 @add(i32 %a, i32 %b) {
entry:  
    %result = add i32 %a, %b  
    ret i32 %result
}
```

* 그 다음 LLVM이 이 IR을 각 CPU의 기계어로 변환

* x86-64 (Intel/AMD):

```
add:    
    lea eax, [rdi + rsi]    
    ret
```

ARM64 (Apple M1):

```
add:
    add w0, w0, w1
    ret
```

* 이런 구조라 보면 된다

```
main.cpp (C++ 소스)
    ↓
[Clang 프론트엔드] - 문법 분석, 타입 체크
    ↓
main.ll (LLVM IR)
    ↓
[LLVM 옵티마이저] - 코드 최적화
    ↓
main.ll (최적화된 LLVM IR)
    ↓
[LLVM 백엔드] - 타겟 CPU에 맞는 기계어 생성
    ↓
main.exe (실행 파일)
```

---

## clang과 llvm

```
C/C++ 소스 코드
       ↓
   [Clang] ← C/C++ 전용 프론트엔드
       ↓
   LLVM IR (중간 언어)
       ↓
   [LLVM] ← 범용 최적화 + 기계어 생성
       ↓
   실행 파일
```

| 구성 요소 | 담당 업무 |
|:---|:---|
| Clang | C/C++/Objective-C 문법 분석, 타입 체크, 에러 메시지 출력, LLVM IR 생성 |
| LLVM | IR 최적화, 각 CPU 아키텍처에 맞는 기계어 생성 |

---

## 난독화와 clang(llvm)

* 독화가 Clang/LLVM에서 주로 되는 이유
    * 정확히 말하면
        * "Clang만 된다"는 것은 정확하지 않다. 정확히는:
        * LLVM 기반 난독화 도구가 많이 개발되어 있다
    * 대표적인 LLVM 난독화 도구:
        * OLLVM (Obfuscator-LLVM)
        * Hikari
        * Armariris
* LLVM에서 난독화가 쉬운 이유
    * LLVM의 "패스(Pass)" 시스템
    * LLVM은 IR을 변환하는 과정을 패스(Pass)라는 단위로 나눔

```
LLVM IR → [패스1] → [패스2] → [패스3] → ... → 최적화된 IR → 기계어
```

* 각 패스는 독립적으로 동작
| 패스 이름 |하는 일 |
|:---|:---|
| DeadCodeElimination | 사용되지 않는 코드 제거 |
| LoopUnroll | 루프 펼치기 |
| Inlining | 함수 인라인화 |
| 사용자 정의 패스 | 원하는 변환 추가 가능 |

* 난독화 = 사용자 정의 패스를 추가하는 것

```
LLVM IR → [기존 패스들] → [난독화 패스] → 난독화된 IR → 기계어
```

