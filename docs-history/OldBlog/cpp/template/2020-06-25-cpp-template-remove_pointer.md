---
title: "C++ Template : remove_pointer"
permalink: cpp/template/remove_pointer/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-25 00:00:00 -0000
last_modified_at: 2020-06-25 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - remove_pointer
  - remove_all_pointer
excerpt: ""
---

```cpp
#include <iostream>
#include <type_traits>  // for is_pointer, remove_pointer
using namepsace std;

template<typename T> void foo(T a)
{
  bool b = is_pointer<T>::value;      // 포인터인지 확인
  typename remove_pointer<T>::type t; // 포인터를 제거한 타입 출력 : int
}

int main()
{
  int n = 10;
  foo(n);
  foo(&n);
}
```

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

template<typename T> 
struct xremove_pointer
{
  typedef T type;
};

template<typename T> 
struct xremove_pointer<T*>  // 포인터로 받게 된다.
{
  typedef T type;
};

template<typename T> void foo(T a)
{
  typename xremove_pointer<T>::type t;
  cout << typeid(t).name() << endl;
}

int main()
{
  int n = 10;
  foo(&n);
}
```

```cpp
// 물론 const, volatile 버전도 만들어야 한다.
template<typename T> 
struct xremove_pointer
{
  typedef T type;
};

template<typename T> 
struct xremove_pointer<T*>
{
  typedef T type;
};

template<typename T> 
struct xremove_pointer<T* const>
{
  typedef T type;
};

template<typename T> 
struct xremove_pointer<T* volatile>
{
  typedef T type;
};

template<typename T> 
struct xremove_pointer<T* const volatile>
{
  typedef T type;
};
```

---

## remove_all_pointer

```cpp
template<typename T> struct xremove_pointer
{
  typedef T type;
};

template<typename T> struct xremove_pointer<T*>
{
  typedef T type;
};

int main()
{
  xremove_pointer<int*>::type n;
  // xremove_pointer<int**>::type n;    // 이건 되나?
  cout << typeid(n).name() << endl;     // int*가 출력된다.
}
```

```cpp
template<typename T> struct xremove_all_pointer
{
  typedef T type;
};

// 재귀로 해결
template<typename T> struct xremove_all_pointer<T*>
{
  typedef xremove_all_pointer<T>::type type;
};

int main()
{
  xremove_all_pointer<int****>::type n;
  cout << typeid(n).name() << endl;     // int
}
```