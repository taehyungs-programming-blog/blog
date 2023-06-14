---
title: "STL : iterator adapter"
permalink: cpp/stl/iterator-adapter/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-10 00:00:00 -0000
last_modified_at: 2020-04-10 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## reverse_iterator 사용

* [Run this code](https://ideone.com/NoWbvR)

```cpp
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

int main()
{
    list<int> s = { 1, 2, 3, 4, 5, 6, 7, 3, 9, 10 };

    auto p1 = begin(s);
    auto p2 = end(s);

    // auto ret = find(p1, p2, 3);
    auto ret = find(p2, p1, 3);         // error
    reverse_iterator<list<int>::iterator> p3(p2);  // p3는 p2와 반대로 동작
    // ++p3 = --p2

    cout << *p3 << endl; // 10
    ++p3;
    cout << *p3 << endl; // 9

    auto ret1 = find(p3, p4, 3);
}
```

---

## reverse_iterator 만들어보기 - 1

```cpp
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

template<typename ITER> class Reverse_iterator
{
    ITER iter;      // list의 일반 반복자
public:
    Reverse_iterator(ITER i) { iter = i; --iter; }
    Reverse_iterator& operator++()
    {
        --iter;
        return *this;
    }
    typename ITER::value_type operator*()
    {
        return *iter;
    }
};

int main()
{
    list<int> s = { 1, 2, 3, 4, 5, 6, 7, 3, 9, 10 };

    auto p1 = end(s);
    Reverse_iterator<list<int>::iterator> p2(p1);
    ++p2;
    cout << *p2 << endl;
}
```

---

## reverse_iterator 좀 더 쉽게 사용하기


* [Run This Code](https://ideone.com/tsyfAa)

```cpp
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

int main()
{
    list<int> s = { 1, 2, 3, 4, 5, 6, 7, 3, 9, 10 };

    auto p1 = begin(s);
    auto p2 = end(s);

    reverse_iterator<list<int>::iterator> r1(p2);       // 가장 기본적 방법
    

    // 좀 더 쉽게 사용하기
    auto r2 = make_reverse_iterator(p2);        
    auto r3 = s.rbegin();
    auto r4 = rbegin(s);

    cout << *r1 << endl;
    cout << *r2 << endl;
    cout << *r3 << endl;
    cout << *r4 << endl;
}
```

---

## find 쉽게 사용하기

* [Run This Code](https://ideone.com/K2gOrV)

```cpp
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

int main()
{
    list<int> s = { 1, 2, 3, 4, 5, 6, 7, 3, 9, 10 };

    auto ret = find(rbegin(s), rend(s), 3);
}
```
