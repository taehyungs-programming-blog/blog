---
title: "(C++) using"
date: 2020-02-03 00:00:00 -0000
---

```cpp
// C에서는 이렇게 사용
typedef int DWORD;
typedef void(*FP)(int);

int main()
{
  DWORD n;  // int
  FP p;     // void(*f)(int)
}
// C++에서는 이렇게 사용
using DWORD = int;
using FP = void(*)(int);

int main()
{
  DWORD n;  // int
  FP p;     // void(*f)(int)
}
```

> 왜 using을 만들었을까?

```cpp
template<typename T> struct Point
{
  T x, y;
};

typedef Point Pixel;           // error
using Pixel = Point<int>;      // ok
```

* using vs typedef
    * typedef : 타입에 대해서만 별칭을 만들 수 있다.
    * using : 타입 뿐 아니라 템플릿에 대한 별칭도 만들 수 있다.
* using 문법 기본 모양
    * using id = type-id;
    * template<template-parameter-list>
    * using id = type-id;

```cpp
// using을 활용해보자.

// 1. type alias(타입에 대한 별칭)
using DWORD = int;

// 2. template alias(템플릿에 대한 별칭)
template<typename T, typename U>
using Duo = pair<T, U>;
Duo<int, double> d1;

template<typename T>
using Ptr = T*;
Ptr<int> p2;    // int*

// 3. 템플릿의 인자를 고정하는 방식
template<typename T>
using Point = pair<T, int>;
Point<int> p; // pair<int, int>

// 4. 복잡한 표현을 좀 더 단순하게.
template<typename T> void foo(T a)
{
  typename remove_pointer<T>::type t;
}
// 위 표현을 좀 더 간단히 해보자.
template<typename T>
using remove_pointer_t = typename remove_pointer<T>::type;
remove_pointer_T<T> t;
```