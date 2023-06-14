---
title: "STL : smart pointer 개념"
permalink: cpp/stl/spointer/basic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-15 00:00:00 -0000
last_modified_at: 2020-04-15 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* [Run This Code](https://ideone.com/a1UZJc)

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Car
{
    int color;
public:
    ~Car() { cout << "~Car()" << endl; }

    void Go() { cout << "Car go" << endl; }
};

int main()
{
    // Car* p = new Car;
    shared_ptr<Car> p(new Car);     // 소멸됨을 확인할 수 있다.

    p->Go();
    (*p).Go();

    // delete p;
}
```
