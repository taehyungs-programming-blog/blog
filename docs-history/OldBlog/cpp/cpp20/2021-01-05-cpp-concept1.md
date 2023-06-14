---
title: "(c++20) concept1"
permalink: cpp/cpp20/concept1/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-05 00:00:00 -0000
last_modified_at: 2021-01-05 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - c++20
category:
  - concept
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## concept 기본

타입이 가져야하는 요구 조건을 정의 하는 문법

```cpp
#include <concepts>

template<typename T>
concept GreaterThan4 = sizeof(T) >= 4;
// T는 반드시 4바이트보다 커야한다.

template<typename T> requires GreaterThan4<T>
void foo(T arg)
{

}

int main()
{
  int i = 10;
  short s = 10;
  double d = 3.4;

  foo(i);   // ok
  foo(s);   // error - short는 4바이트보다 작다
  foo(d);   // ok
}
```

---

## require 상세

```cpp
#include <type_traits>

template<typename T>
void foo(T arg)
{
}

template<typename T> requires std::is_integral_v<T>
void goo(T arg)
{
}

int main()
{
  foo(1);   // foo(int arg)
  foo(3.4); // foo(double arg)

  goo(1);   // is_integral을 만족 ok
  goo(3.4); // error - double은 만들지 못함
}
```

---

## require and error msg

```cpp
#include <type_traits>

template<typename T> T gcd1(T a, T b)
{
  return b == 0 ? a : gcd1(b, a % b);
}

template<typename T> requires std::is_integral_v<T>
T gcd2(T a, T b)
{
  return b == 0 ? a : gcd2(b, a % b);
}

int main()
{
  gcd1(4, 2);
  gcd1(4.2, 2.1);   // error!

  gcd2(4, 2);
  gcd2(4.2, 2.1);   // error! -> 하지만 error 메시지가 더 정확히 나옴!
}
```

```cpp
#include <type_traits>

double gcd1(double a, double b) { return 0; }

template<typename T> T gcd1(T a, T b)
{
  return b == 0 ? a : gcd1(b, a % b);
}

int main()
{
  gcd1(4.2, 2.1);   // double 버전이 호출
  gcd1(4, 2);       // template 버전이 호출
  gcd1(4.2f, 2.1f); // float 버전이 없으니 template버전이 호출 - %연산에서 에러가 발생
}
```

```cpp
#include <type_traits>

double gcd2(double a, double b) { return 0; }

template<typename T> requires std::is_integral_v<T>
T gcd2(T a, T b)
{
  return b == 0 ? a : gcd2(b, a % b);
}

int main()
{
  gcd2(4.2, 2.1);     // double 호출
  gcd2(4, 2);         // teplate 호출
  gcd2(4.2f, 2.1f);   // is_integral에 걸리기에 double호출됨
}
```

좀 어렵게 말해서 위 기술을 SFINAE(Substitution Failure Is Not An Error)라 한다.

---

## overloading

```cpp
#include <type_traits>

void foo(int a) {}
void foo(double a) {}
void foo(double a, double b) {}

// 가상함수가 있는 타입
void goo() {}

// 가상함수가 없는 타입
void goo() {}

// 특정조건에 따라 함수오버로딩 해보자
```

```cpp
#include <iostream>
#include <type_traits>

class Test
{
public:
  virtual void f() {}
};

template<typename T> requires std::is_polymorphic_v<T>
void foo(const T& arg)
{
  std::cout << "가상함수가 있는 타입에 대한 알고리즘" << std::endl;
}

template<typename T> requires (!std::is_polymorphic_v<T>)
void foo(const T& arg)
{
  std::cout << "가상함수가 없는 타입에 대한 알고리즘" << std::endl;
}

int main()
{
  foo(Test());    // 가상함수가 있는 타입에 대한 알고리즘
}
```

```cpp
#include <iostream>
#include <type_traits>

template<typename T> requires !(std::is_pointer_v<T>)
void printv(const T& arg)
{
  std::cout << arg << std::endl;
}

template<typename T> requires std::is_pointer_v<T>
void printv(const T& arg)
{
  // 포인터일땐 주소까지 출력
  std::cout << arg << " : " << *arg << std::endl;
}

int main()
{
  int n = 0;
  printv(n);
  printv(&n);
}
```

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <concepts>

int main()
{
  //std::vector c = {1,2,3,4,5,6,7,8,9,10};
  std::list c =  {1,2,3,4,5,6,7,8,9,10};
  auto p = std::begin(c);

  p = p + 5;  // error : list는 덧셈을 지원하지 않음.

  std::advance(p, 5); // C++17 : 덧셈 연산이 가능하면 덧셈으로, 안되면 ++로 연산해준다
  // 이걸 만들어 보자

  std::cout << *p << std::endl;
}
```

```cpp
// 임의 접근이 가능할 경우
template<typename T> requires std::random_access_iterator<T>
void advance(T& p, int n)
{
  std::cout << "임의접근" << std::endl;
  p = p + n;
}

template<typename T> requires std::input_iterator<T>
void advance(T& p, int n)
{
  std::cout << "임의접근 아닐경우" << std::endl;
  while(n--) ++p;
}
```

---

## requires clauses (require 전략)

```cpp
#include <type_traits>

bool check() { return true; }

template<typename T> requires true
void f1(T a) {}

template<typename T> requires false   // 항상 미사용
void f2(T a) {}

template<typename T> requires std::is_pointer_v<T>  // 포인터일때만 사용
void f4(T a) {}

template<typename T> requires 1       // error : 이런 표현은 불가능
void f5(T a) {}

template<typename T> requires (check())   // 컴파일시간에 알수있는 함수만 가능
void f5(T a) {}

template<typename T> requires (sizeof(T)>4)   // 조건도 들어감
void f6(T a) {}
```

```cpp
#include <type_traits>

template<typename T> requires (sizeof(T) > 1)
void f1(T a)
{
}

template<typename T> requires (sizeof(T) > 1)
void f2(T a)
{
}

int main()
{
  f1(0);
  f2(0);
}
```

조건을 만족하는 template이 2개 이상있다면?

```cpp
#include <iostream>

template<typename T> requires (sizeof(T) > 1)
void foo(T a)
{
  std::cout << "A" << std::endl;
}

template<typename T> requires (sizeof(T) > 1 && sizeof(T) < 8)
void foo(T a)
{
  std::cout << "B" << std::endl;
}

int main()
{
  foo(3.4);   // A
  foo(3);     // error - foo의 호출이 애매모호
}
```