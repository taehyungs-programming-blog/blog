---
title: "C++ Template : 함수 리턴타입구하기"
permalink: cpp/template/funcreturntype/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-27 00:00:00 -0000
last_modified_at: 2020-04-27 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - return type
excerpt: ""
---

```cpp
#include <iostream>
using namespace std;

double hoo(short a, int b) { return 0; }

template<typename T> struct result_type
{
  typedef T type;
};

template<typename R, typename A1, typename A2> 
struct result_type<R(A1, A2)>     // 만약 인자가 여러개라면??
{
  typedef R type;
};

template<typename T> void foo(const T& t)
{
  typename result_type<T>::type ret;
  cout << typeid(ret).name() << endl;
}

int main()
{
  foo(hoo);
}
```

```cpp
template<typename T> struct result_type
{
  typedef T type;
};

template<typename R, typename ... Types> 
struct result_type<R(Types...)> 
{
  typedef R type;
};

template<typename T> void foo(const T& t)
{
  typename result_type<T>::type ret;
  cout << typeid(ret).name() << endl;
}
```

```cpp
template<typename T> struct result_type
{
  // typedef T type;
  // 함수가 아닐경우 에러처리
  static_assert(is_function<T>::value, "error");
};

template<typename R, typename ... Types> 
struct result_type<R(Types...)> 
{
  typedef R type;
};

template<typename T> void foo(const T& t)
{
  typename result_type<T>::type ret;
  cout << typeid(ret).name() << endl;
}
```
