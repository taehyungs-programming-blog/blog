---
title: "C++ Template : type modification"
permalink: cpp/template/modification/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-26 00:00:00 -0000
last_modified_at: 2020-04-26 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## remove_pointer

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

template<typename T> void foo(T a)
{
    bool b = is_pointer<T>::value;
    typename remove_pointer<T>::type t;

    cout << typeid(t).name() << endl;
}

int main()
{
    int n = 10;
    foo(n);     // int
    foo(&n);    // int
}
```

remove_pointer 만들기

```cpp
template<typename T> struct xremove_pointer
{
    typedef T type;
};

template<typename T> struct xremove_pointer<T*>
{
    typedef T type;
};
```

필요성을 그렇게 못느끼겠음 ... 그냥 remove_pointer가 있다는 것 정도만 알면 될거 같음..

---

