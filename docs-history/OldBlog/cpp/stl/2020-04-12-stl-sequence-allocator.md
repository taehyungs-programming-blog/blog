---
title: "STL : allocator"
permalink: cpp/stl/sequence/allocator/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-12 00:00:00 -0000
last_modified_at: 2020-04-12 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## allocator 사용하기

```cpp
#include <iostream>
#include <memory>       // for allocator
#include <vector>
using namespace std;

struct Point
{
    int x, y;

    Point(int a = 0;, int b = 0) : x(a). y(b)
    {
        std::cout <<"Point() : " << x << ", " << y << std::endl;
    }
    ~Point() { std::cout << ~"Point()" << std::endl; }
};

int main()
{
    Point* p = new Point[2];
    allocator<Point> a;
    Point* p = a.allocate(2);       // 할당
    a.construct(p);                 // 생성자 호출
    a.construct(p+1, 1, 2);         // 매개변수를 넣어 생성자 호출
    a.destroy(p);                   // 소멸자 호출
    a.deallocate(p, 2);             // 해지
}
```

```cpp
vector<Point> v;    // vector<Point, allocator<Point>> // 이런 의미이다.

v.get_allocator();      // vector가 내부적으로 사용하는 allocator 호출
```
