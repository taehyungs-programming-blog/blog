---
title: "C++ Template : template"
permalink: cpp/template/template/                # link 직접 지정
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
  - template
excerpt: ""
---

```cpp
class Test
{
public:
  template<typename T> static void f() {}
  template<typename T> class Complex {}
};

template<typename T> void foo(T a)
{
  Test::f<int>();   // okay
  T::f<int>();      // error - f가 template일지 아닐지 모르니 에러가 발생
  T::template f<int>();  // okay - f가 template이라고 확실히 알려주면 okay
  
  Test::Complex<int> c1;    // okay
  T::Complex<int> c2;       // error  - T가 자료형인지 모른다
  typename T::Complex<int> c3;  // error - Complex가 template인지 모른다
  typename T::template Complex<int> c4;   // ok
}

int main()
{
  Test t;
  foot(t);
}
```