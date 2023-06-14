---
title: "STL concept"
permalink: cpp/stl/concept/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-04 00:00:00 -0000
last_modified_at: 2020-04-04 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <algorithm>
using namespace std;

struct Point
{
    int x, y;
};

int main()
{
    int n1 = min(1, 2);
    int n2 = mint({1,2,3,4,5});

    Point p1, p2;
    min(p1, p2);        // 이게 되게 만들어보자. -> concept(C++20부터 지원)
}
```

* Concept - 특정 함수(클래스)를 사용하기 위해 타입이 가져야 하는 조건
* min 함수에 전달되는 인자는 LessThanComparable을 만족해야한다.