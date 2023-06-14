---
title: "STL : 사용자 정의 타입과 컨테이너"
permalink: cpp/stl/sequence/customtype/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-12 00:00:00 -0000
last_modified_at: 2020-04-12 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <vector>
using namespace std;

class Point
{
public:
    int x, y;
    // 디폴트 생성자가 없다.
    Point(int a, int b) : x(a), y(b) {}
};

int main()
{
    vector<Point> v1;
    vector<Point> v2(10);               // error
    vector<Point> v3(10, Point(0,0)); // ok

    v3.resize(20);              // error
    v3.resize(20, Point(0,0));  // ok

    sort(begin(v3), end(v3));   // error - 어떻게 정렬할지 모른다.

    sort(begin(v3), end(v3), [](const Point& p1, const Point& p2){return p1.x > p2.x;}); // ok
}
```

```cpp
// 비교연산자를 제공하는 것도 하나의 방법
class Point
{
public:
    int x, y;
    // 디폴트 생성자가 없다.
    Point(int a, int b) : x(a), y(b) {}
    bool  operator<(const Point& p) const
    {
        return x < p.x;
    }
    bool operator==(const Point^ p) const
    {
        return x == p.x;
    }
};
```

```cpp
// relation operators를 열어두는 것도 방법
#include <iostream>
#include <vector>
using namespace std;
using namespace std::rel_ops;       // 단, < ==은 사용자가 만들어야함, 나머지 연산자는 여기서 만들어 줌.
```

---

## 사용자 정의 타입과 컨테이너 사용시 주의사항

```cpp
#include <iostream>
#include <vector>
using namespace std;

class Point
{
    int x, y;
public:
    Point(int a, int b) : x(a), y(b) { cout << "Point()" << endl; }
    ~Point() { cout << "~Point()" << endl; }
    Point(const Point&) { cout << "Point(const Point&)" << endl; }
};

int main()
{
    vector<Point> v;

    Point p1(10, 20);       // 생성자 호출
    v.push_back(p1);        // 복사 생성자 호출    
    // 소멸자 2회 호출 

    // 모두 오버헤드이다.

    cout << "----------" << endl;
}
```

```cpp
int main()
{
    vector<Point> v;

    // 임시객체로 넣기 -> 역시 복사가 이루어 진다.
    v.push_back(Point(10, 20));
    // ...
```

```cpp
// 객체 자체를 vector가 만들게 한다. -> 그냥 이거 쓰자
v.emplace_back(10, 20);
v.emplace_front(10, 20);
```

```cpp
v.push_back(move(p1));  // move도 가능!
```