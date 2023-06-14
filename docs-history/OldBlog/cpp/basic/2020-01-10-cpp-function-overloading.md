---
title: "(C++) 함수 오버로딩 고급"
date: 2020-01-10 00:00:00 -0000
---

### 목차

* [name mangling](#name-mangling)
* [인자 기반 탐색(ADL(Argument Dependent Lookup))](#인자-기반-탐색(ADL(Argument-Dependent-Lookup)))
* [](#)
* [](#)

---

### name mangling

> 컴파일러가 컴파일 시간에 (같은이름의)함수 이름을 변경하는 것을 name mangling(name decoration)이라고 한다.

```cpp
int square(int a)
{
     return a * a;
}

double square(double a)
{
    return a * a;
}

int main()
{
    square(3);
    square(3.3);
}
```

> 문제는 C언어에서는 name mangling을 지원하지 않는것에서 발생한다.
> 
> 예를 들어보자.

```cpp
// main.cpp
#include "square.h"

int main()
{
    square(3);
    // cpp로 생성된 main이 square라는 함수를 호출하려한다.
}
```

```cpp
// square.h
int square(int a);
```

```cpp
// square.c
// 그런데 square가 c로 만들어졌다면??
// main문에서 찾지 못하는 문제가 발생
int square(int a)
{
    return a * a;
}
```

---

#### 해결책 = extern "C"

> C++ 컴파일러에게 현재 함수(변수)가 C언어로 작성되었다고 알려주는 지시어.

```cpp
// square.h
#ifdef __cplusplus
extern "C" {
#endif

int square(int a);

#ifdef __cplusplus
}
#endif
```

---

### 인자 기반 탐색(ADL(Argument Dependent Lookup))

```cpp
namespace Video
{
    struct Card
    {
        // ...
    };
    
    void init(Card card)
    {
    }
}

int main()
{
    Video::Card card;
    init(card);   // error가 나오지 않음 -> ??
    // 인자 기반 탐색! -> card를 보고 Video의 init을 호출
}
```

---

### 인자 기반 탐색의 필요성

```cpp
namespace Shape
{
  class Point
  {
        int x;
        int y;
  public:
        Point(int a = 0, int b = 0) : x(a), y(b) {}
        friend Point operator+(const Point& p1, const Point& p2);
    };
    
    Point operator+(const Point& p1, const Point& p2)
    {
        return Point(p1.x+p2.x, p1.y+p2.y);
    }
}

int main()
{
    Shape::Point p1(1,1);
    Shape::Point p2(2,2);
    
    Shape::Point p3 = p1 + p2;
    // 인자 기반 탐색이 없다면
    // Shape::Point p4 = operator+(p1, p2);   // 불편하다
}
```