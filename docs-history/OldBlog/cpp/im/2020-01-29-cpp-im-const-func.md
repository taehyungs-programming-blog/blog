---
title: "(C++) 상수 멤버 함수의 필요성"
date: 2020-01-29 00:00:00 -0000
---

* 상수객체는 상수 멤버함수만 호출할 수 있다.

```cpp
#include <iostream>
using namespace std;

class Point
{
public:
  int x, y;
  
  Point(int a = 0, int b = 0) : x(a), y(b) {}
  
  void set(int a, int b)
  {
    x = a;
    y = b;
  }
  
  void print() const // 상수 멤버 함수(내부에서 변수의 수정이 불가능)
  // 그런데? 잘 사용하지 않는 기능중 하나이다.
  // 단순히 코드의 안전성을 위해 사용해야 할까?
  {
    cout << x << ", " << y << endl;
  }
};

int main()
{
  Point p(1, 1);
}
```

```cpp
int main()
{
  const Point p(1, 1);
  
  // 상수 개체기에 수정이 불가능
  p.x = 10;       // error
  p.set(10, 20);  // error
  
  p.print();      // error
  // 컴파일러 입장에서는 print함수가 내부에서 쓰고있는지 확인할 길이 없다 -> 상수 멤버함수의 필요성!
}
```

```cpp
class Point
{
public:
  int x, y;
  
  Point(int a = 0, int b = 0) : x(a), y(b) {}
  
  void set(int a, int b)
  {
    x = a;
    y = b;
  }
  
  // 컴파일러에게 이 함수는 상수함수라고 알려주는 역할을 한다.
  void print() const;
```

---

## 그런데 상수 객체는 많이 쓰나?

```cpp
struct Rect
{
  int ox, oy, width, height;
  
  Rect(int x = 0, int y = 0, int w = 0, int h = 0)
    : ox(x), oy(y), width(w), height(h) {}
    
  int getArea() { return width* height; }
};

int main()
{
  Rect r(0, 0, 10, 10);
  
  int n = r.getArea();
}
```

* 이런경우 많이 사용된다. 특정함수 foo라는 함수에서 r을 부르려할때 메모리 복사의 오버헤드를 피하고자 const 참조로 부르는 경우가 많다.

```cpp
void foo(const Rect& r)
{
  int n = r.getArea();    // error!
  // 상수 객체는 상수 멤버 함수만 호출가능함을 잊지말자!
}
```

> 해결책

```cpp
struct Rect
{
  int ox, oy, width, height;
  
  Rect(int x = 0, int y = 0, int w = 0, int h = 0)
    : ox(x), oy(y), width(w), height(h) {}
    
  int getArea() const { return width* height; }
};
```

* 객체의 상태를 변경하지 않는 모든 멤버함수는
* 반드시 const 멤버 함수가 되어야 한다.