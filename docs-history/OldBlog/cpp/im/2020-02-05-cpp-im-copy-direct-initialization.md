---
title: "(C++) uniform constructor"
date: 2020-02-05 00:00:00 -0000
---

```cpp
int main()
{
  int n1 = 0;   // copy initialization
  int n2(0);    // direct dinitialization
  
  int n3 = { 0 };
  int n4{ 0 };
}
```

> 둘의 차이점은 뭘까?

```cpp
class Point
{
  int x, y;
public:
  Point() : x(0), y(0) {}
  Point(int a) : x(0), y(0) {}
  Point(int a, int b) : x(0), y(0) {}
};

int main()
{
  Point p1(5);      // ok
  Point p2 = 5;     // ok, but 생성자가 explicit이라면 error발생
  
  Point p3(1, 1);   // ok
  Point p4 = (1, 1);// error
  
  Point p5{ 1, 1 };     // ok
  Point p6 = { 1, 1 };  // ok, but 생성자가 explicit이라면 error
  
  Point p7;
  Point p8{};           // direct
  Point p9 = {};        // copy
}
```

```cpp
// 만약 생성자에 explicit을 걸면?
class Point
{
  int x, y;
public:
  explicit Point() : x(0), y(0) {}
  explicit Point(int a) : x(0), y(0) {}
  explicit Point(int a, int b) : x(0), y(0) {}
};

int main()
{
  Point p1(5);      // ok
  Point p2 = 5;     // error
  
  Point p3(1, 1);   // ok
  Point p4 = (1, 1);// error
  
  Point p5{ 1, 1 };     // ok
  Point p6 = { 1, 1 };  // error
  
  Point p7;
  Point p8{};           // ok
  Point p9 = {};        // error
}
```