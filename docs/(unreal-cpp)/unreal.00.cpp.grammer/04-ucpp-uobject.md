---
layout: default
title: "04. Unreal Object"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## UnrealObject는 왜 필요할까?

* 우선, 왜 C++인가 를 먼저 짚고 가자
* C++
    * 메모리를 직접 제어 가능
    * Cache 활용 극대화
    * 저수준 API 직접 호출
    * 복사 작업의 최소화
* 그 외 언어
    * 유지보수성 향상
    * Crash로 부터 보호
    * 자동 메모리 관리
    * 고질적 실수 예방

* **Unreal** - 흠... 그 외 언어의 기능도 나빠보이지 않는데 ... 같이 쓸순 없나?
    * UnrealObject, Unreal C++의 탄생!

### UnrealObject의 특징?

* 클래스 기본 객체(CDO) - 클래스의 기본 값과 타입정보 제공
* 리플렉션(Reflection) - 런타임에서 클래스 정보의 참조 가능
* 인터페이스(Inteface) - 모던 객체 지향 언어가 제공하는 인터페이스의 제공
* 향상된 열거형(Enum) -
* 델리게이트(Deligate) - 객체간의 결합을 낮출 수 있는 델리게이트 기능의 제공
* 가비지컬렉션(Garbage Collection) - 자동 메모리 관리
* 향상된 구조체(Struct) - 리플렉션이 가능한 구조체의 지원
* 직렬화(Serialization) - 객체 정보를 바이트 스트림으로 저장, 전송 로딩이 가능

---

## UnrealObject는 어떻게 만들까?

```cpp
UCLASS()                // UCLASS() 매크로를 톻해 UnrealObject임을 알린다
class UNREALOBJECT_API  // UNREALOBJECT_API 다른 모듈에서 UMyObject를 참조하게 도와준다
UMyObject : public UObject
{
	GENERATED_BODY()    // UnrealObject의 특징을 이용하기 위해 Include 됨
};
```
