---
title: "C++ Template : remove_all_pointer"
permalink: cpp/template/remove_all/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-26 00:00:00 -0000
last_modified_at: 2020-04-26 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

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
    xremove_pointer<int*>::type n;      // int
    xremove_pointer<int**>::type n1;      // ?? -> int*가 나오게 된다.
    cout << typeid(n).name() << endl;
}
```

```cpp
// 재귀적 방법으로 해결가능
template<typename T> struct xremove_all_pointer
{
    typedef T type;
};

template<typename T> struct xremove_all_pointer<T*>
{
    typedef typename xremove_all_pointer<T>::type type;
};

int main()
{
    xremove_all_pointer<int**>::type n;      
    cout << typeid(n).name() << endl;
}
```

