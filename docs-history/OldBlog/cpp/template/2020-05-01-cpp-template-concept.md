---
title: "C++ Template : Concept"
permalink: cpp/template/concept/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-01 00:00:00 -0000
last_modified_at: 2020-05-01 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
struct Point
{
    int x, y;
};

template<typename T> T Min(T x, T y)
{
    return y < x ? y : x;
}

int main()
{
    Point p1, p2;
    Min(p1, p2);        // error - Point는 크기비교가 불가능
    // 해결해보자
}
```

Concept를 활용!

* 타입이 가져야 하는 조건을 코드로 표현하는 것을 말한다.
* Concept은 C++20에 도입이 될 예정이다.

```cpp
struct Point
{
    int x, y;
    bool operator<(const Point& p) { return true; }
};

// 1. concept만들기
template<typename T>
concept bool LessThanComparable = requires(T a, T b)
{
    { a < b } -> bool;
    // 비교연산이 가능하고 bool을 리턴할수 있어야한다.
};

// T는 LessThanComparable를 만족해야 사용이 가능하다.
template<typename T> requires LessThanComparable
T Min(T x, T y)
{
    return y < x ? y : x;
}

int main()
{
    Point p1, p2;
    Min(p1, p2);
}
```

```cpp
// 2. template 만들때 concept 표기
// T자리 대신에 concept을 넣는다
LessThanCompare Min(TLessThanCompare x, LessThanCompare y)
{
    return y < x ? y : x;
}
```