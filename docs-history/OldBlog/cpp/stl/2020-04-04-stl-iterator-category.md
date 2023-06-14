---
title: "STL : iterator category"
permalink: cpp/stl/iterator-category/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-04 00:00:00 -0000
last_modified_at: 2020-04-04 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

int main(){
    vector<int> v = { 1, 3, 5, 7, 9, 10 };
    list<int> v = { 1, 3, 5, 7, 9, 10 };        // error!
    // vecor는 되는데 list는 에러가 난다

    sort(begin(v), end(v));

    for(auto& n : v)
        cout << n << ", ";
}
```

왜 list는 에러가 날까?<br>
우선 아래를 먼저 이해하자.

```cpp
#include <forward_list>
#include <list>
using namespace std;

int main()
{
    forward_list<int> s1 = { 1, 2, 3 };
    // forward_list는 single list(단방향 리스트)이다

    auto p = begin(s1);

    ++p;
    --p;        // forward_list이기에 구현을 하지 않음.
}
```

음... 컨테이너의 종류(반복자 카테고리)에 따라 갖은 기능이 다를 수 있구나<br>

```cpp
#include <iostream>
#include <list>
using namespace std;

int main()
{
    list<int> s1 = { 1, 2, 3 };

    auto i = begin(s1);

    int n = *i;     // 입력이라 표기
    *i = 20;        // 출력이라 표기
}
```

* list : ++연산은 제공 +는 어차피 속도의 향상이 없기에 제공하지 않음
* vector : ++, +연산 모두 제공
* deque : ++, +연산 모두 제공

```cpp
#include <iostream>
#include <list>
using namespace std;

int main()
{
    list<int> s1 = { 1, 2, 3 };

    auto i1 = begin(s1);
    auto i2 = i1;

    if(i1 == i2)
    {
        cout << (*i1 == *i2) << endl;
        cout << (++i1 == ++i2) << endl;
    }
    // 두 개 이상의 iterator가 하나의 컨테이너를 가르킬 수 있는지? -> multipass guarantee라 한다.
```

