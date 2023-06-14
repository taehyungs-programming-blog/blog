---
title: "STL : move iterator"
permalink: cpp/stl/move-iterator/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-10 00:00:00 -0000
last_modified_at: 2020-04-10 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## move iterator 만들기

* C++ 11에서 추가된 기능임을 기억하자

```cpp
#include <iostream>
#include <iterator>
#include <vector>
#include "People.h"
using namespace std;

int main()
{
    vector<People> v;
    v.push_back(People("A"));
    v.push_back(People("B"));
    v.push_back(People("C"));
    v.push_back(People("D"));

    cout << "---------------" << endl;
    auto p1 = begin(v);

    People peo1 = *p1;      // 복사 생성자
    move_iterator<vector<People>::iterator> p2(p1);
    //People peo2 = *p2;      // move 생성자
    People peo3 = move(*p1);// move 생성자

    auto p3 = make_move_iterator(begin(v));
}
```

```cpp
vector<People> v2(begin(v), end(v));
vector<People> v2(make_move_iterator(begin(v)), make_move_iterator(end(v)));
```