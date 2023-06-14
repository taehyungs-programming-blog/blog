---
title: "C++ Template : using recursive"
permalink: cpp/template/recursive/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-27 00:00:00 -0000
last_modified_at: 2020-04-27 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <tuple>
using namespace std;

template<typename ... Types> void foo(Types ... args)
{
    tuple<Types ...> tp(args...);

    cout << get<0>(tp) << endl;
    cout << get<1>(tp) << endl;
    cout << get<2>(tp) << endl;
}

int main()
{
    foo(1, 3.4, "AA");  // args : 1, 3.4, "AA"
}
```

```cpp
#include <iostream>
#include <tuple>
using namespace std;

void foo() {}   // 재귀의 종료를 위해서

template<typename T, typename ... Types>
void foo(T value, Types ... args)
{
    cout << value << endl;      // 1
    foo(args...);               // 이런식으로 recursive하게 호출해서 쓰는 방법!
}

int main()
{
    foo(1, 3.4, "AA");
}
```