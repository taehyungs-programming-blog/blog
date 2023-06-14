---
title: "STL : sequence container1 - array"
permalink: cpp/stl/sequence/array/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-12 00:00:00 -0000
last_modified_at: 2020-04-12 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* 핵심 : vector와 array는 동일한거 같은데... 왜 쓸까?

```cpp
#include <iostream>
#include <vector>
#include <array>
using namespace std;

int main()
{
    int x[5];           // stack에 있음

    vector<int> v(5);   // heap에 있음

    // container는 무조건 heap에 메모리를 잡나?
    // Nope

    array<int, 5> a = {1,2,3,4,5};    // stack에 잡는다.

    a.push_back(10);        // error - stack에 들어있기에  뒤에 넣을 수 없다.(앞도 마찬가지...)
}
```

```cpp
array<int, 5> a = {1,2,3,4,5};
auto p = find(++a.begin(), a.end(), 3); // error
auto p = find(next(a.begin()), a.end(), 3); // ok
```