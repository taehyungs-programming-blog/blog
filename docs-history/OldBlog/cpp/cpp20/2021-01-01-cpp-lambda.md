---
title: "(c++20) lambda"
permalink: cpp/cpp20/lambda/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-01 00:00:00 -0000
last_modified_at: 2021-01-01 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - c++20
category:
  - lambda
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## 개발환경

* VS2019 Preview

```s
# 빌드하기
cl 소스이름.cpp /std:c++latest
```

![](/file/image/cpp20-lambda-1.png)

---

## C++20에 추가된 새로운 키워드

* Concept 문법 : `concept`, `requires`
* Coroutine 문법 : `co_await`, `co_return`, `co_yield`
* Module 문법 : `module`, `import`, `export`
* 상수성 지원 : `constinit`, `consteval`
* UTF-8 문자 : `char8_t`

---

## Example

```cpp
#include <iostream>
#include <numbers>  // c++20 최신 헤더

int main()
{
  std::cout << std::numbers::pi << std::endl;
  std::cout << std::numbers::e << std::endl;
  std::cout << std::numbers::sqrt2 << std::endl;
}
```

![](/file/image/cpp20-lambda-2.png)

굳이 define하지 않아도 된다.

---

## feature test macro

* 현재 사용중인 컴파일러가 특정 문법 또는 라이브러리 지원여부를 확인가능

```cpp
#include <iostream>
#include <span>
#include <chrono>

int main()
{
    // 언어 특징 지원 여부 조사
#ifdef __cpp_concepts
  // concept이라는 문법을 지원
    std::cout << "support concepts" << std::endl;
#endif

#ifdef __cpp_modules
  // modules이라는 문법을 지원
    std::cout << "support modules" << std::endl;
#endif

    // 라이브러리 특정 지원 여부 조사 (헤더필요)
#ifdef __cpp_lib_span
    std::cout << "support span lib" << "std::endl";
#endif

#ifdef __cpp_lib_chrono 
    std::cout << "support chrono lib" << std::endl;
    std::cout << "chrono version : " << __cpp_lib_chrono << std::endl;
    // cout해보면 버전정보를 의미한다.
#endif
}
```

![](/file/image/cpp20-lambda-3.png)

---

## 여기서 부터 lambda!

## C++20의 lambda 추가기능

* 람다표현식에서 템플릿 사용 가능
* 평가 되지 않은 표현식에서 람다 표현식 사용가능
* 캠쳐하지 않은 람다 표현식에서 디폴트 생성자와 대입연사자 사용가능
* 암시적인 this, 캠쳐가 deprecated 됨
* parameter pack 캡쳐 가능

이렇게는 무슨말인지 모르니 자세한 설명은 아래 참조!

---

## 람다표현식에서 템플릿 사용 가능

```cpp
#include <iostream>

auto add1 = [](int a, int b) { return a + b; };
auto add2 = [](auto a, auto b) { return a + b; }; // generic lambda expression -> C++14부터 가능

int main()
{
  std::cout << add1(1, 2) << std::endl;       // 3
  std::cout << add1(1.1, 2.2) << std::endl;   // 3 -> error!

  std::cout << add2(1, 2) << std::endl;       // 3
  std::cout << add2(1.1, 2.2) << std::endl;   // 3.3
  std::cout << add2(1, 2.2) << std::endl;     // 3.2 -> 이걸 에러내고 싶다면??(입력을 같은 타입만 받게 사용하고 싶다)
}
```

```cpp
auto add3 = [](auto a, decltype(a) b) { return a + b; };

int main()
{
  std::cout << add3(1, 2) << std::endl;       // 3
  std::cout << add3(1.1, 2.2) << std::endl;   // 3.3
  std::cout << add3(1, 2.2) << std::endl;     // error!
}
```

```cpp
#include <iostream>

auto add1 = [](auto a, auto b) { return a + b; };       // C++14
auto add2 = []<typename T>(T a, T b) { return a + b; }; // C++20

int main()
{
  std::cout << add1(1, 2) << std::endl;       // 3
  std::cout << add1(1.1, 2.2) << std::endl;   // 3.3
  std::cout << add1(1, 2.2) << std::endl;     // 3.2

  std::cout << add2(1, 2) << std::endl;       // 3
  std::cout << add2(1.1, 2.2) << std::endl;   // 3.3
  std::cout << add2(1, 2.2) << std::endl;     // error!
}
```

---

## 평가 되지 않은 표현식에서 람다 표현식 사용가능

```cpp
#include <iostream>
#include <memory>

struct Freer
{
  inline void operator()(void* p) const noexcept
  {
    std::cout << "free" << std::endl;
    free(p);
  }
};

int main()
{
  std::unique_ptr<int> up1(new int);    // 안전하게 delete 가 불린다.

  //std::unique_ptr<int> up2(static_cast<int*>(malloc(100)));  // free를 통해야 delete가 된다.
  std::unique_ptr<int, Freer> up2(static_cast<int*>(malloc(100)));  // 소멸자를 지정해 줘야한다.
}
```

이걸 좀 람다표현식으로 쓸 수 없나?

```cpp
std::unique_ptr<int, [](int* p){free(p);}> up2(static_cast<int*>(malloc(100)));
// 이렇게 표현하고 싶은데 그냥 람다식은 안들어가고 자료형이 들어가야한다.
std::unique_ptr<int, decltype([](int* p){free(p);})> up2(static_cast<int*>(malloc(100)));
// 이게 C++20부터는 선언가능
```

```cpp
#include <iostream>
#include <memory>

int add(int a, int b) { return a + b; }

int main()
{
  std::cout << sizeof(int) << std::endl;        // 4
  std::cout << sizeof(add(1,2)) << std::endl;   // 4 (리턴타임을 의미)

  decltype(add(1,2));   // int n : 표현식 결과의 타입

  std::cout << sizeof( [](int a, int b) { return a+b; }) << std::endl;  // 이건 뭐가 나올까?
  // C++17까지는 error
  // C++20은 동작한다.

  std::cout << sizeof( [](int a, int b) { return a+b; }) << std::endl;    // 1
  std::cout << sizeof( [](int a, int b) { return a+b; }(1,2)) << std::endl; // 4

}
```

---

## 캠쳐하지 않은 람다 표현식에서 디폴트 생성자와 대입연사자 사용가능

```cpp
#include <iostream>

int main()
{
  int v1 = 10;
  auto f1 = [](int a, int b){ return a + b; }

  //                        C++11 ~ C++17           C++20
  decltype(f1) f2;        // Error                  ok (디폴트 생성자)
  decltype(f1) f3 = f1;   // ok                     ok (복사 생성자)
  f3 = f1;                // Error                  ok (대입 연산자)
}
```

```cpp
#include <iostream>

int main()
{
  int v1 = 10;
  auto f1 = [v1](int a, int b){ return a + b; }

  //                        C++11 ~ C++17           C++20
  decltype(f1) f2;        // Error                  Error (디폴트 생성자)
  decltype(f1) f3 = f1;   // ok                     ok (복사 생성자)
  f3 = f1;                // Error                  Error (대입 연산자)
}
```

---

## 암시적인 this, 캠쳐가 deprecated 됨

```cpp
#include <iostream>
#include <functional>

struct Sample
{
  int value = 0;
  auto foo()
  {
    int n = 10;

    // 멤버함수에서 [=]의 의미는 [=] 뿐만아니라 this까지 capture
    auto f = [=](int a) { return a + n + value; };  // value라는 멤버데이터는 this가 capture되기에 접근이 가능
    std::cout << sizeof(f) << std::endl;
    return f;
  }
};
std::function<int(int)> f;
void goo()
{
  Sample s;
  f = s.foo();
}
int main() 
{ 
  goo(); 
  std::cout << f(10) << std::endl;    // C++17 - 컴파일은 되지만 쓰레기 값이 나온다. - goo에서 생성된 Sample은 소멸된지 오래!
  // C++20 - warning이 나타난다 - 암묵적 this를 쓰지말라고 격오나옴
}
```

---

## parameter pack 캡쳐 가능

```cpp
#include <iostream>

// Capture Parameter pack by value.
template<typename ... Args> auto f1(Args&&... args)
{
  return [...args = std:forward<Args>(args)](){ (std::cout << ... << args); };
}

// Capture Parameter pack by reference
template<typename ... Args> auto f2(Args&&... args)
{
  return [&...args = std::forward<Args>(args)](){ (std::cout << ... << args); };
}

int main()
{
  f1(1,2,3,4,5)();

  int a = 1, b = 2, c = 3;
  f2(a,b,c)();
}
```