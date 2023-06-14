---
title: "C++ Template : int2type"
permalink: cpp/template/int2type/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-24 00:00:00 -0000
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
  - int2type
excerpt: ""
---

```cpp
void foo(int n) {}
void foo(double d) {}

int main()
{
  foo(3);       // foo(int)
  foo(3.4);     // foo(double)
  
  // 아래를 void foo(double d)를 호출하게 할 순 없나?
  foo(0);
  foo(1);
}
```

```cpp
template<int N> struct int2type
{
  enum { value = N };
};

void foo(int n) {}

int main()
{
  int2type<0> t0;
  int2type<1> t1;
  // t0와 t1은 다른 타입이 된다.
  /*
  // int2type<0>
  template<0> struct int2type
  {
    enum { value = 0 };
  };
  
  // int2type<1>
  template<1> struct int2type
  {
    enum { value = 1 };
  };
  */
  
  foo(t0);
  foo(t1);
  // 둘은 다른 함수를 부르게 된다. -> 함수오버로딩을 유발할 수 있다.
}
```

어떻게 쓸까?

```cpp
#include <iostream>
using namespace std;

template<typename T> struct xis_pointer
{
  static constexpr bool value = false;
};

template<typename T> struct xis_pointer<T*>
{
  static constexpr bool value = true;
};

template<typename T> void printv(T v)
{
  if (xis_pointer<T>::value)    // if문은 런타임에 if문을 확인하기에 아래코드를 모두 컴파일하게 된다.
    cout << v << " : " << *v << endl;   // 컴파일 중 여기서 *v 참조를 할 수 없음.
  else
    cout << v << endl;
}

int main()
{
  int n = 3;
  printv(n);  // error
  printv(&n);
}
```

```cpp
template<typename T> void printv(T v)
{
  if constexpr (xis_pointer<T>::value)  // C++17에서 이렇게 쓰면 간단하게 해결되긴 한다.
    cout << v << " : " << *v << endl;
  else
    cout << v << endl;
}

// C++17이 아니라면?
```

```cpp
// 사용이 되어야지만 아래 코드가 생성됨을 기억하자(지연된 인스턴스)
// 그런데 이렇게 한다 하더라도 ... 결국 컴파일 시간에 아래 코드가 생성되며 에러가 발생..
template<typename T> void printv_pointer(T v)
{
  cout << v << " : " << *v << endl;
}

template<typename T> void printv_not_pointer(T v)
{
  cout << v << endl;
}

template<typename T> void printv(T v)
{
  if (xis_pointer<T>::value)  // 컴파일 타임에 결정할 수 없기에 아래 코드를 생성
    printv_pointer(v);
  else
    printv_not_pointer(v);
}

int main()
{
  int n = 3;
  printv(n);
  printv(&n);
}
```

```cpp
template<typename T> void printv_imp(T v, int2type<1>)
{
  cout << v << " : " << *v << endl;
}

template<typename T> void printv_imp(T v, int2type<0>)
{
  cout << v << endl;
}

template<typename T> void printv(T v)
{
  printv_imp(v, int2type<xis_pointer<T>::value>());
  // int2type<1>() = 새로운 타입
}

int main()
{
  int n = 3;
  printv(n);
  printv(&n);
}
```