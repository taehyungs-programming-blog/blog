---
title: "STL : iterator-basic"
permalink: cpp/stl/iterator-basic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-04 00:00:00 -0000
last_modified_at: 2020-04-04 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## Lecture Note

* [Lecture Note : 반복자의 무효화](https://ideone.com/z0WeSw)

---

```cpp
#include <iostream>
#include <list>
using namespace std;

int main()
{
    list<int> s = { 1, 2, 3, 4, 5 };

    // list<int>::iterator p = s.begin();
    // auto p = s.begin();
    auto p = begin(s);      // 이 구조가 제일 좋음 - C++11
    // 배열에도 먹히기 때문이다.

    int n = size(s);

    // 이거 주의
    auto p2 = end(s);
    *p2 = 10;       // error - end는 마지막 요소가 아니라 마지막 다음 요소임을 기억하자.
}
```

---

## 반복자의 무효화

```cpp
#include <iostream>
#include <list>
#include <vector>
using namespace std;

int main()
{
    vector<int> v = {1,2,3,4,5};

    auto p = begin(v);

    v.resize(100);      // 버퍼 재할당

    cout << *p << endl;     // error - 버퍼가 재할당되며 무효화 된다.
}
```