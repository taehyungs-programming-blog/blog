---
layout: default
title: "02. Unreal C++ 코딩규칙"
parent: "(문법)"
grand_parent: "(Unreal C++ 🚀)"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [Unreal Coding Standard](https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/)
    * 참고 [Google Coding Standard](https://google.github.io/styleguide/)

---

## 접두사 **U**
    
* 언리얼 Object (`UObject`)를 상속받는 경우 접두사 **U**를 붙여준다.

```cpp
// UMyGameInstance도 접두사 U가 붙은것을 알수 있다.
UCLASS()
class HELLOUNREAL_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	virtual void Init() override;
};
```

* 새 C++ 클래스를 추가하면 모두 Object를 부모로 두기 때문이다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/unreal/basic_cpp/basic_cpp_2-1.png"/>
</p>

---

## 예외의 케이스

* Actor
    * Actor는 UObject를 상속 받기는 하나, 접두사 **A**를 붙여준다.
* Interface
* SWidget
* bool
* 일반 C++ 클래스 - 접두사 **F**를 붙여준다.

---

## 기본 int를 지원하지 않는다

* `int8`, `int16` 등 정확히 명시해야 한다.
* `float`, `double`은 지원

---

## std 표준라이브러리 사용?

* 기본원칙은 std표준라이브러리를 사용하지 않는게 원칙
* 팀원들과 협의하에 사용해도 된다.

---

## 될수 있으면 const를 사용해 달라

```cpp
oid SomeMutatingOperation(FThing& OutResult, const TArray<Int32>& InArray)
{
    // InArray will not be modified here, but OutResult probably will be
}

void FThing::SomeNonMutatingOperation() const
{
    // This code will not modify the FThing it is invoked on
}

TArray<FString> StringArray;
for (const FString& : StringArray)
{
    // The body of this loop will not modify StringArray
}
```

```cpp
// 포인터를 사용할때도 const를 붙이면 좋은데
T* const Ptr = ...;

// 참조형은 const를 붙이면 안된다. (어차피 수정안될테니)
T& const Ref = ...;
```

---

* 모두 정리하는것은 비효율! 사이트를 참조하자

## 그래도 정리하자면 ...

* 명명규칙
    * 파스칼 케이싱을 사용한다
    * 변수에 공백 및 언더스코어가 없다
    * 모든 클래스와 구조체는 고유의 접두사를 포함한다
* 코드의 명확성
    * 파라미터에 가급적 In, Out 접두사를 명시
    * const를 활용
    * 레퍼런스를 통한 복사는 안된다
    * auto는 가급적 사용하지 않는다