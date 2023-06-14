---
title: "STL : iterator helper"
permalink: cpp/stl/iterator-helper/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-10 00:00:00 -0000
last_modified_at: 2020-04-10 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## iterator 보조 함수 사용하기

* iterator는 아래의 다섯 보조 함수를 지원한다
    * next
    * prev
    * advance
    * distance
    * iter_swap

```cpp
#include <iostream>
#include <iterator>
#include <forward_list>
using namespace std;

int main()
{
    int x[10] = { 1,2,3,4,5,6,7,8,9,10 };

    forward_list<int> s = { 1,2,3,4,5,6,7,8,9,10 };

    auto p1 = next(begin(s));   // 2가 나오는데 ... 이게 왜 필요하지??

    // 아래를 보자.

    cout << *p1 << endl;
}
```

```cpp
#include <iterator>
using namespace std;

int* foo()
{
    static int x[10] = { 1,2,3,4,5,6,7,8,9,10 };
    return x;
}

int main()
{
    ++foo();       // error
     // 이런 연산이 가능하게 만들고 싶다
}

// 이렇게 받으면 된다.
int main()
{
    auto p2 = next(foo()); // ok!
    // ++p2;   
}
// 포인터 리턴을 받으며 생기는 문제인데 next로 받으면 문제가 없어진다.
```

```cpp
#include <iostream>
#include <iterator>
#include <forward_list>
using namespace std;

int main()
{
    int x[10] = { 1,2,3,4,5,6,7,8,9,10 };

    forward_list<int> s = { 1,2,3,4,5,6,7,8,9,10 };

    auto p1 = next(begin(s));

    advance(p1, 3);

    cout << *p1 << endl;        // 5
    cout << distance(besing(s), p1) << endl; // p1 - begin(s) // 4

    iter_swap(p1, begin(s));
    cout << *p1 << endl;        // 1
}
```