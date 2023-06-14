---
title: "(C++) brace-init & conversion"
date: 2020-02-03 00:00:00 -0000
---

```cpp
class Point
{
  int x, y;
public:
  explicit Point(int a, int b) : x(a), y(b) {}
};

void foo(Point p) {}

int main()
{
  foo({1, 1});          // explicit으로 error
  Point p1(1, 1);
  
  // brace-init, C++11부터 나온 초기화 방법
  Point p2{1, 1};       // direct initialize
  Point p3 = {1, 1};    // copy initialize - explicit으로 error
}
```