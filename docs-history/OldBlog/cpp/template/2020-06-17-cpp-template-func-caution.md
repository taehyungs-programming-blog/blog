---
title: "C++ Template : 함수 템플릿 호출시 주의 사항"
permalink: cpp/template/func-caution/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-25 00:00:00 -0000
last_modified_at: 2020-06-17 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - 주의사항
excerpt: ""
---

```cpp
#include <iostream>
using namespace std;

class Point
{
  int x, y;
public:
  Point(int a = 0, int b = 0) : x(a), y(b) {}
};

int main()
{
  Point p1(1, 2);
  
  cout << p1 << endl;   // 요걸 쓰고 싶다
}
```

```cpp
#include <iostream>
using namespace std;

class Point
{
  int x, y;
public:
  Point(int a = 0, int b = 0) : x(a), y(b) {}
  
  friend ostream& operator<<(ostream& os, const Point& p);
};

ostream& operator<<(ostream& os, const Point& p)
{
  return os << p.x << ", " << p.y;
}
```

만약 Point가 template이라면??

```cpp
#include <iostream>
using namespace std;

template<typename T>
class Point
{
  T x, y;
public:
  Point(T a = 0, T b = 0) : x(a), y(b) {}
  
  friend ostream& operator<<(ostream& os, const Point<T>& p);
};

template<typename T>
ostream& operator<<(ostream& os, const Point<T>& p)
{
  return os << p.x << ", " << p.y;
}

int main()
{
  Point<int> p1(1, 2);
  
  cout << p1 << endl;   // error! ???
}
```

우선 왜 에러가 나오는지 보자..

```cpp
#include <iostream>
using namespace std;

template<typename T> void foo(T a)
{
  cout << "T" << endl;
}

void foo(int a)
{
  cout << "int" << endl;
}

int main()
{
  foo(3);   // int - 정확한 타입이 우선된다.
}
```

```cpp
#include <iostream>
using namespace std;

template<typename T> void foo(T a)
{
  cout << "T" << endl;
}

void foo(int a);

int main()
{
  foo(3);   // error - 정확한 선언의 구현부가 없기에 링크에러 발생
}
```

```cpp
#include <iostream>
using namespace std;

template<typename T> class Point{};

template<typename T> void foo(Point<T> a)
{
  cout << "T" << endl;
}

void foo(Point<int> a) { cout << "int" << endl; }

int main()
{
  Point<int> p;
  foo(3);   // int - 역시 정확한 버전을 사용함.
}
```

```cpp
#include <iostream>
using namespace std;

template<typename T>
class Point
{
  T x, y;
public:
  Point(T a = 0, T b = 0) : x(a), y(b) {}
  
  friend ostream& operator<<(ostream& os, const Point<T>& p);
};

// 함수 템플릿
template<typename T>
ostream& operator<<(ostream& os, const Point<T>& p)
{
  return os << p.x << ", " << p.y;
}

int main()
{
  Point<int> p1(1, 2);  // 여기가 호출 되는 순간 Point<int>를 생성하게 된다.
  
  cout << p1 << endl;   // error! ???
}
```

```cpp
class Point<int>
{
  int x, y;
public:
  Point(int a = 0, int b = 0) : x(a), y(b) {}
  
  friend ostream& operator<<(ostream& os, const Point<int>& p);
  // 이렇게 된 순간 operator<<는 Point<int>로 확정이 되어 버린다.
  // friend ostream& operator<<(ostream& os, const Point<int>& p);
  // 를 찾을수 없기에 에러가 발생하는 것.
};
```

뭐 … 그래서 어떻게 해결하면 좋나??

```cpp
#include <iostream>
using namespace std;

template<typename T>
class Point
{
  T x, y;
public:
  Point(T a = 0, T b = 0) : x(a), y(b) {}
  
  // friend ostream& operator<<(ostream& os, const Point<T>& p);
  // 함수 템플릿을 friend로 선언함으로 해결가능.
  template<typename U>
  friend ostream& operator<<(ostream& os, const Point<U>& p)
};

// 함수 템플릿
template<typename T>
ostream& operator<<(ostream& os, const Point<T>& p)
{
  return os << p.x << ", " << p.y;
}

int main()
{
  Point<int> p1(1, 2);  // 여기가 호출 되는 순간 Point<int>를 생성하게 된다.
  
  cout << p1 << endl;   // error! ???
}
```

그리 좋은 방법은 아닌듯 … 더 좋은 방법은 없나?

```cpp
#include <iostream>
using namespace std;

template<typename T>
class Point
{
  T x, y;
public:
  Point(T a = 0, T b = 0) : x(a), y(b) {}
  
  // friend ostream& operator<<(ostream& os, const Point<T>& p);
  // 구현을 클래스 내부에 한다.
  friend ostream& operator<<(ostream& os, const Point<T>& p)
  {
    return os << p.x << ", " << p.y;
  }
};

// 함수 템플릿
template<typename T>
ostream& operator<<(ostream& os, const Point<T>& p)

int main()
{
  Point<int> p1(1, 2);  // 여기가 호출 되는 순간 Point<int>를 생성하게 된다.
  
  cout << p1 << endl;   // error! ???
}
```
