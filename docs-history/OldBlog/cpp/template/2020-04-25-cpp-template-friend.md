---
title: "C++ Template : template과 friend"
permalink: cpp/template/friend/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-25 00:00:00 -0000
last_modified_at: 2020-04-25 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iotream>
using namespace std;

class Point
{
    int x, y;
public:
    Point(int a = 0, int b = 0) x(a), y(b) {}

    friend ostream& operator<<(ostream& os, const Point& p);
};

ostream& operator<<(ostream& os, const Point& p)
{
    return os << p.x << ", " << p.y;
}

int main()
{
    Point p(1, 2);
    cout << p << endl;
}
```

```cpp
template<typename T>
class Point
{
    T x, y;
public:
    Point(T a = 0, T b = 0) x(a), y(b) {}

    friend ostream& operator<<(ostream& os, const Point<T>& p);
};

template<typename T>
ostream& operator<<(ostream& os, const Point<T>& p)
{
    return os << p.x << ", " << p.y;
}

int main()
{
    Point<int> p(1, 2);     // error - why??
    cout << p << endl;      
}
```

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
    foo(3);     // int가 호출됨.(명확한 선언이 먼저 호출된다.)
}
```

```cpp
template<typename T>
class Point
{
    T x, y;
public:
    Point(T a = 0, T b = 0) x(a), y(b) {}

    // T = int로 결정이 되면 아래 코드는 변경이 된다.
    friend ostream& operator<<(ostream& os, const Point<T>& p);
    // friend ostream& operator<<(ostream& os, const Point<int>& p);    // 이렇게!
};

// 따라서 함수 템플릿으로는 링크시킬 수 없게 된다.

// 함수 템플릿.
template<typename T>
ostream& operator<<(ostream& os, const Point<T>& p)
{
    return os << p.x << ", " << p.y;
}

int main()
{
    Point<int> p(1, 2);
    cout << p << endl;      
}
```

해결은 어떻게 하나??

```cpp
template<typename T>
class Point
{
    T x, y;
public:
    Point(T a = 0, T b = 0) x(a), y(b) {}

    // friend ostream& operator<<(ostream& os, const Point<T>& p);
    template<typename U> 
    friend ostream& operator<<(ostream& os, const Point<U>& p);
};

// 함수 템플릿.
template<typename T>
ostream& operator<<(ostream& os, const Point<T>& p)
{
    return os << p.x << ", " << p.y;
}

int main()
{
    Point<int> p(1, 2);         // okay
    cout << p << endl;      
}
```

```cpp
template<typename T>
class Point
{
    T x, y;
public:
    Point(T a = 0, T b = 0) x(a), y(b) {}

    // 구현을 클래스 내부에 하라
    friend ostream& operator<<(ostream& os, const Point<T>& p)
    {
        return os << p.x << ", " << p.y;    
    }
};

int main()
{
    Point<int> p(1, 2);         // okay
    cout << p << endl;      
}
```