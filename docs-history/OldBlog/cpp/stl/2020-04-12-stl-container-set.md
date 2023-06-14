---
title: "STL : set"
permalink: cpp/stl/associative/set/                # link 직접 지정
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
#include <set>
using namespace std;

int main()
{
    set<int> s;

    s.push_front(10);       // error
    s.insert(30);
    s.insert(40);
    s.insert(20);
    s.insert(10);
    s.insert(45);
    s.insert(25);

    auto p = begin(s);      // 10 - 왼쪽 마지막 맆

    *p = 10;            // error - 반복자로 값을 변경 불가
    while( p != end(s) )
    {
        cout << *p << endl;
        ++p;
    }

    auto p2 = find(begin(s), end(s), 10);
    // 일반 find를 사용할 경우 선형검색을 해서 성능이 나쁘다.
    auto p3 = s.find(10);       // ok
}
```

```cpp
set<int> s;

s.insert(30);
s.insert(40);
s.insert(20);
s.insert(10);
s.insert(45);
s.insert(25);

s.insert(20);       // error - set은 같은 값이 두 번 들어가지 않는다.

auto ret = s.insert(20);
if( ret.second == false)
    cout << "fail" << endl;
```

```cpp
// 중복을 허용하고 싶다면
multiset<int> s;
```

## set에 사용자 정의 타입 넣기

```cpp
#include <iostream>
#include <set>
using namespace std;

struct Point
{
    int x, y;
    Point(int a = 0, int b = 0) : x(a), y(b) {}
};

struct PointLess
{
    bool operator()(const Point& p1, const Point& p2) const
    {
        return p1.x < p2.x;
    }
}

int main()
{
    set<Point, PointLess> s;

    s.insert(Point(1,1));

    s.emplace(1,3);     // 이렇게 넣는것을 추천...
}
```