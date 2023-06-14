---
title: "STL : unordered"
permalink: cpp/stl/associative/unordered/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-14 00:00:00 -0000
last_modified_at: 2020-04-14 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* unordered는 hash table기반으로 구현되어 있기에 검색속도가 빠르다.

## hash func

```cpp
#include <iostream>
#include <string>
#include <functional>       // for hash func
using namespace std;

int main()
{
    hash<int> hi;
    hash<double> hd;

    cout << hi(50) << endl;     // hash값이 출력된다.
}
```

---

## unordered set

* [Run This Code](https://ideone.com/1qh5Pu)

```cpp
#include <iostream>
#include <string>
#include <functional>
#include <unordered_set>
using namespace std;

int main()
{
    unordered_set<int> s;

    s.insert(30);
    s.insert(40);
    s.insert(20);
    s.insert(10);
    s.insert(45);
    s.insert(25);

    for(auto n : s)
        cout << n << endl;

    // set은 '<'로 비교
    // unordered_set 'hash함수'로 비교

    // set : sort가 되어 있다
    // unordered_set : sort(정렬)이 되어 있지 않다.
```

---

## unordered 사용자 타입

```cpp
#include <iostream>
#include <string>
#include <functional>
#include <unordered_set>
using namespace std;

struct Point
{
    int x, y;
    Point(int a = 0, int b = 0) : x(a), y(b) {}
};

int main()
{
    set<Point> s;           // Point에 '<' operator필요
    unordered_set<Point> s;

    s.insert(Point(1,1));   // hash함수를 필요로 한다.
    s.insert(Point(2,2));   // 비교를위해 == operator도 필요하다
    s.insert(Point(3,3));

    s.find(Point(1,1));
```

```cpp
// Point의 해쉬 함수 객체
struct PointHash
{
    size_t operator()(const Point& p) const
    {
        hash<int> hi;
        return hi(p.x) + hi(p.y);
    }
};

struct PointEqual
{
    bool operator()(const Point& p1, const Point& p2) const
    {
        return p1.x == p2.x && p1.y == p2.y;
    }
};

int main()
{
    unordered_set<Point, PointHash, PointEqual> s;
```