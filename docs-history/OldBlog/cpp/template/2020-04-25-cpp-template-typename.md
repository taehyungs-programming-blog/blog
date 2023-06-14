---
title: "C++ Template : typename"
permalink: cpp/template/typename/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-25 00:00:00 -0000
last_modified_at: 2020-06-18 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - 주의사항
excerpt: ""
---

```cpp
class Test
{
public:
  enum { value1 = 1 };
  static int value2;
  
  typedef int INT;
  using SHORT = short;    // C++ 11
  
  class innerClass {};
};

int Test::value2 = 1;

int main()
{
  // 클래스 내부의 호출은 두 가지 호출이 존재
  // 값을 호출
  int n1 = Test::value1;
  int n2 = Test::value2;
  
  // 타입을 호출
  Test::INT a;
  Test::SHORT b;
  Test::innerClass c;
}
```

```cpp
int p = 0;

class Test
{
public:
  // enum { DWORD = 5 }
  // typdef int DWORD;
};

template<typename T>
int foo(T t)
{
  T::DWORD * p;   // T는 값으로 해석이 될까 타입으로 해석이 될까?
  // 결론적으론 컴파일러는 값으로 해석을 한다.
  
  typename T::DWORD * p;    // typename을 지정해 줘야 타입이라고 컴파일러에게 알려줄 수 있다.
  return 0;
}

int main()
{
  Test t;
  foo(t);
}
```