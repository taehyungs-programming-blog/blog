---
title: "(C++) uniform constructor"
date: 2020-02-05 00:00:00 -0000
---

```cpp
struct Point
{
  int x, y;
};

class Complex
{
  int re, im;
public:
  Complex(int r, int i) : re(r), im(i) {}
};

int main()
{
  /* 초기화 할 때 한 가지 방법을 쓰자.
  int n = 0;
  int x[2] = { 1, 2 };
  Point p = { 1, 2 };
  Complex c(1, 2);
  */
  
  int n{ 0 };
  int x[2]{ 1, 2 };
  Point p{ 1, 2 };
  Complex c{ 1, 2 };
  
  // 아래와 동일한 표현이다.
  int n = { 0 };
  int x[2] = { 1, 2 };
  Point p = { 1, 2 };
  Complex c = { 1, 2 };
}
```

```cpp
int n2 = { 3.4 };   // error
char c1{ 300 };     // error
char c2{ 100 };     // ok
```