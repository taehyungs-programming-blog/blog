---
layout: default
title: "39. SFINAE"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* [참고 🌍](https://modoocode.com/255)

* **SFINAE(Substitution Failure Is Not An Error)** 무슨 말 일까?

```cpp
// 아래와 같은 Template을 만들었다.
template <typename T>
typename T::value_type negate(const T& t) {
    return -T(t);
}
```

```cpp
// 내가 이렇게 써버리면 에러일까?
int negate(int i) { return -i; }
```

* 결론부터 말하자면 에러가 아니다

```cpp
// 이렇게 int ::value_type라는 이상한 애를 쓰는데 에러가 아니라고??
int ::value_type negate(const int& t) {
    /* ... */
}
```

* 에러가 아니다!
    * C++에선 표준으로 Substitution Failure Is Not An Error을 정해두었다.
    * **치환 실패는 오류가 아니다** - 치환으로 인한 에러는 에러처리를 하지 않겠다는 말.
* 무슨소리 ?? 아래는 에러던데?

```cpp
template <typename T>
void negate(const T& t) {
    typename T::value_type n = -t();
}
```

* 위는 에러이다. 내부에서 `T::value_type`를 쓰는경우 치환에러로 치지 않는다 (C++ 표준사항)

---

## Example

* 클래스 내부에 특정함수(`serialize()`)가 포함되어 있는지 컴파일 타임에 알고 싶다면?

```cpp
#include <iostream>
#include <type_traits>

// has_serialize를 체크하는 헬퍼 구조체 선언
template<typename T, typename = void>
struct has_serialize : std::false_type {};

// has_serialize 특수화. serialize 메서드를 가진 타입에 대해 true를 반환합니다.
template<typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>> : std::true_type {};

// 테스트 클래스들
class WithSerialize {
public:
    void serialize() {}
};

class WithoutSerialize {
};

// 테스트 함수
template <typename T>
void test() {
    if constexpr (has_serialize<T>::value) {
        std::cout << "Has serialize method" << std::endl;
    } else {
        std::cout << "Does not have serialize method" << std::endl;
    }
}

int main() {
    test<WithSerialize>();    // 출력: Has serialize method
    test<WithoutSerialize>(); // 출력: Does not have serialize method
    return 0;
}

```

* SFINAE 원칙은 "Substitution Failure Is Not An Error"를 의미합니다. 이 원칙에 따라, 템플릿 인스턴스화 과정에서 타입 대체(substitution)가 실패해도 오류로 간주되지 않습니다. 대신, 컴파일러는 해당 템플릿을 무시하고 다른 후보를 찾습니다.
* 예시에서 `std::void_t<decltype(std::declval<T>().serialize())>` 표현식은 타입 T가 serialize 메서드를 가지고 있는지를 검사합니다. `std::declval<T>()`는 타입 T의 인스턴스를 생성하지 않고도 T의 멤버 함수를 호출할 수 있게 해주며, decltype은 이 메서드 호출의 반환 타입을 추론합니다.
* 타입 T가 serialize 메서드를 가질 때: 이 표현식은 유효하며, `std::void_t<decltype(std::declval<T>().serialize())>`는 void 타입으로 치환됩니다. 이 경우, SFINAE는 적용되지 않으며, `has_serialize<T, void>` 템플릿 특수화가 활성화됩니다.
* 타입 T가 serialize 메서드를 가지고 있지 않을 때: `std::declval<T>().serialize()` 표현식은 유효하지 않습니다. 이 경우, SFINAE 원칙에 따라 이는 오류로 간주되지 않고, 해당 템플릿 특수화는 무시됩니다. 그 결과, 첫 번째 `has_serialize<T, ...>` 템플릿 정의가 사용됩니다.
* 이 방식으로, SFINAE는 특정 조건을 만족하는 타입에 대해서만 특정 템플릿 코드를 활성화시킬 수 있게 해줍니다. 이는 타입 특성에 따라 다른 동작을 하는 코드를 작성할 때 매우 유용합니다.
* 유사하게 enable_if를 사용하기도 함.