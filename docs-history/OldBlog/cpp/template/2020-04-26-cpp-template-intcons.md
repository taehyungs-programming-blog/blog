---
title: "C++ Template : integral_constant"
permalink: cpp/template/intcons/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-26 00:00:00 -0000
last_modified_at: 2020-06-24 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - integral_constant
excerpt: ""
---

```cpp
// int2type의 발전된 형태이다.
#include <iostream>
using namespace std;

template<int N> struct int2type   // int말고 다른 타입은 안되나? char, short 등 ...
{
  enum { value = N };
};

int main()
{
  
}
```

```cpp
template<typename T, T N> struct integral_constant
{
  static constexpr T value = N;
};

integral_constant<int, 0> t0;
integral_constant<short, 0> t1;

// 아래는 자주 사용되기에 c++에서 typedef을 둬서 보통 쓴다.
typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

// is_pointer만들때
template<typename T>
struct is_pointer : false_type
{
};

template<typename T>
struct is_pointer<T*> : true_type
{
};
```

```cpp
#include <iostream>
#include <type_trais> // for integral_constant
using namespace std;

template<typename T>
// void printv_imp(T v, int2type<1>)
void printv_imp(T v, true_type)
{
  cout << v << " : " << *v << endl;
}

template<typename T>
// void printv_imp(T v, int2type<0>)
void printv_imp(T v, false_type)
{
  cout << v << endl;
}

template<typename T>
void printv(T v)
{
  // printv_imp(v, int2tpye<xis_pointer<T>::value>());
  printv_imp(v, is_pointer<T>());
}

int main()
{
  int n = 3;
  printv(n);
  printv(&n);
}

// 코드 가독성의 향상을 가져온다.
```