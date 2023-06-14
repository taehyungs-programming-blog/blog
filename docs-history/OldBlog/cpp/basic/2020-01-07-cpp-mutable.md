---
title: "(C++) mutable"
#permalink: cpp/basic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-07 00:00:00 -0000
last_modified_at: 2020-03-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

상수 멤버 함수 안에서도 값을 변경하고 싶다.

```cpp
#include <iostream>

class Point
{
  int x, y;
  mutable int cnt = 0;
public:
  Point(int a = 0, int b = 0) : x(a), y(b) {}
  
  void print() const
  {
    ++ cnt;   // 상수함수인데 변경가능.
    std::cout << x << ", " << y << std::endl;
  }
};

int main()
{
  Point pt(1, 1);
  pt.print();
  pt.print();
}
```