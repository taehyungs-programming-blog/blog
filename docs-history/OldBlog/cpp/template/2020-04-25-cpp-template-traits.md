---
title: "C++ Template : IfThenElse"
permalink: cpp/template/traits/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-25 00:00:00 -0000
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
  - IfThenElse
excerpt: ""
---

```cpp
#include <iostream>
#include <typeinfo>
using namespace std;

template<bool b, typename T, typename F> struct IfThenElse
{
  typedef T type;
};

template<typename T, typename F> 
struct IfThenElse
{
  typedef F type;
};

int main()
{
  IfThenElse<true, int, double>::type t0;    // int
  IfThenElse<false, int, double>::type t1;  // double
  // 컴파일 시간에 bool값에 따라서 타입을 결정하게 해준다.
  
  cout << typeid(t0).name() << endl;
  cout << typeid(t1).name() << endl;
}
```

```cpp
#include <iostream>
using namespace std;

template<bool b, typename T, typename F> struct IfThenElse
{
  typedef T type;
};

template<typename T, typename F> 
struct IfThenElse
{
  typedef F type;
};

template<size_t N>
struct Bit
{
//   int bitmap;
  using type = IfThenElse<(N <= 8), char, int>::type;
  type bitmap;
};

int main()
{
  Bit<32> b1;
  Bit<8> b2;
  
  cout << sizeof(b1) << endl;   // 4
  cout << sizeof(b2) << endl;   // 1
}
```

```cpp
template<size_t N>
struct Bit
{
  using type = typename conditional <(N <= 8), char, int>::type;
  // 동일한 동작을 한다.
  type bitmap;
};
```