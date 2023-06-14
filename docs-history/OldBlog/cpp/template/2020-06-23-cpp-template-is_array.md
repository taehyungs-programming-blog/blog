---
title: "C++ Template : is_array"
permalink: cpp/template/is_array/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-23 00:00:00 -0000
last_modified_at: 2020-06-23 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - is_array
excerpt: ""
---

```cpp
#include <iostream>
using namespace std;

template<typename T> void foo(T& a)
{
  if(is_array<T>::value)  // is_array는 이미 C++에 있지만 공부삼아 만들어 본다.
    cout << "array" << endl;
  else
    cout << "not array" << endl;
}

int main()
{
  int x[3] = {1,2,3};
  foo(x);
}
```

```cpp
template<typename T> struct xis_array
{
  static constexpr bool value = false;
};

template<typename T, size_t N> struct xis_array<T[N]>   // 배열의 타입은 int[#]이기에
{
  static constexpr bool value = true;
};

template<typename T> struct xis_array<T[]>   // 크기를 알수없는 배열에 대해서 처리
{
  static constexpr bool value = true;
};
```

응용

```cpp
template<typename T> struct xis_array
{
  static constexpr bool value = false;
  static constexpr size_t size = -1;
};

template<typename T, size_t N> struct xis_array<T[N]>
{
  static constexpr bool value = true;
  // 배열을 전달할때 배열의 크기 까지 전달이 가능하다
  static constexpr size_t size = N;
};

template<typename T> struct xis_array<T[]> 
{
  static constexpr bool value = true;
};

// xis_array<T>::size로 사용가능
```

```cpp
// 참고로 C++에서는 배열의 크기를 구하는 함수를 지원
extent<T,0>::value
```