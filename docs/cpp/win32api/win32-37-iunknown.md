---
layout: default
title: "37. IUnknown"
parent: (Windows API)
grand_parent: (C++)
nav_order: 4
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## IUnknown 란?

* **IUnknown**은 COM의 모든 인터페이스의 기본 클래스
* 세 가지 핵심 메서드를 제공
    * `QueryInterface()` : 다른 인터페이스를 쿼리
    * `AddRef()` : 참조 카운트 증가
    * `Release()` : 참조 카운트 감소 (0이면 객체 해제)

---

## 왜 쓸까?

### DLL 경계의 위험성

* 일반 C++ 클래스를 DLL로 넘기면 이런 문제가 생긴다

```cpp
// DLL에서 만든 객체
MyClass* obj = CreateObject();  // DLL의 힙(heap)에서 메모리 할당

// EXE에서 삭제하면?
delete obj;  // 💥 EXE의 힙에서 해제 시도 → 크래시!
```

* DLL과 EXE는 각각 다른 힙(메모리 풀)을 사용할 수 있다. 그래서:
    * DLL에서 new한 건 DLL에서 delete해야 함
    * EXE에서 new한 건 EXE에서 delete해야 함
* COM의 Release()는 "만든 놈이 지운다" 원칙을 강제. 객체가 자기 자신을 delete this로 지우니까 항상 올바른 힙에서 해제.

#### shared_ptr을 쓰면 안 되나?

* 같은 문제가 생깁니다!

```cpp
// DLL에서 반환
std::shared_ptr<MyClass> GetObject();

// EXE에서 받아서 사용 후 소멸...
// shared_ptr의 deleter가 EXE의 delete를 호출 → crash
```

* 더 큰 문제는
    * STL 버전 불일치 : DLL은 VS2019로 빌드, EXE는 VS2022로 빌드하면 shared_ptr 내부 구조가 다를 수 있음
    * Debug/Release 불일치 : Debug 빌드의 shared_ptr과 Release 빌드의 것은 크기가 다름
    * CRT(C Runtime) 불일치 : /MD, /MT 옵션에 따라 메모리 관리자가 다름

### 컴파일러/버전 문제

```cpp
// 일반 C++ 클래스
class MyRenderer {
    std::string name;        // VS2019 vs VS2022 레이아웃 다름
    std::vector<int> data;   // Debug vs Release 크기 다름
    virtual void Render();
};
```

* 위 클래스를 DLL로 내보내면:
    * VS2019로 빌드한 DLL + VS2022로 빌드한 EXE = crash
    * Debug DLL + Release EXE = crash

* COM 인터페이스는:

```cpp
interface IRenderer : public IUnknown {
    virtual void Render() = 0;  // 순수 가상 함수만!
};
```