---
title: "(C++) 상수 멤버 함수"
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

상수 객체는 상수 멤버 함수만 사용할 수 있다.

```cpp
#include <iostream>

class Point
{
public:
  int x, y;
  Point(int a = 0, int b = 0) : x(a), y(b) {}
  void set(int a, int b) { x = a; y = b; }
  void print()
  {
    x = 10;            
    std::cout << x << ", " << y << std::endl;   
  }
};

int main()
{
  const Point pt(1,1);    // 상수 객체
  pt.x = 10;      // error
  pt.set(10, 10); // error
  pt.print();     // error - 상수 객체는 상수 멤버 함수만 호출 가능!
}
```

```cpp
void print() const
{
    x = 10;            
    std::cout << x << ", " << y << std::endl;   
}
```

---

## 상수 멤버 함수의 필요성

```cpp
class Rect
{
  int xpos, ypos, width, height;
public:
  Rect(int x = 0, int y = 0, int w = 0, int h = 0) : 
    xpos(x), ypos(y), width(w), height(h) {}
    
  // int getArea() { return width * height; }
  int getArea() const { return width * height; }
};

// void foo( Rect r )
void foo( const Rect& r )   // r이 상수 객체가 된다.
{
  int n = r.getArea();      // error - r이 상수 객체이기에 getArea도 상수 멤버 함수여야한다.
}

int main()
{
  Rect r(1, 1, 10, 10);
  
  int n = r.getArea();
  foo(r);
}
```

객체의 상태를 변경하지 않는 모든 멤버 함수는 반드시 상수 멤버 함수로 만들어야 한다.

---

```cpp
class Test
{
  int data;
public:
  // int* getData() const;
  const int * getData() const
  {
    // data는 상수 이다.
    return &data;
  }
};
```