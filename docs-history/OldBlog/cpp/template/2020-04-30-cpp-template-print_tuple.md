---
title: "C++ Template : print_tuple"
permalink: cpp/template/print_tuple/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-30 00:00:00 -0000
last_modified_at: 2020-04-30 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <tuple>
using namespace std;

// template<size_t ... N> struct index_sequence {};

template<typename TP, size_t ... I> void print_tuple(const TP& tp, const index_sequence<I...>&)     // I : 0, 1, 2
{
    int x[] = { get<I>(tp)... };    // get<0>(tp), get<1>(tp), get<2>(tp)
    for(auto& n : x)
        cout << n << ", ";
}

int main()
{
    tuple<int, int, int> tp(1, 2, 3);
    // print_tuple(tp, index_sequence<0, 1, 2>());        // 1, 2, 3
    print_tuple(tp, index_sequence<3>());
}
```

```cpp
template<typename TP> void print_tuple<const TP& tp>
{
    print_tuple_imp(tp, make_index_squence<tuple_size<TP>::value>());
}
```

