---
title: "STL : algorithm 4가지 변형"
permalink: cpp/stl/algorithm4way/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-11 00:00:00 -0000
last_modified_at: 2020-04-11 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## find_if 사용하기

* [Run This Code](https://ideone.com/MRF9ua)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

bool foo(int a) { return a % 3 == 0; }

int main()
{
    vector<int> v = { 6, 9, 3, 1, 2 };

    auto p = find( begin(v), end(v), 3);


    // 만약 3의 배수(조건)을 검색하고 싶다면?
    auto p1 = find_if(begin(v), end(v), foo);

    auto p2 = find_if(begin(v), end(v), [](int a){return a % 3 == 0;});

    cout << *p << endl;
}
```

---

## remove_if 사용

* [Run This Code](https://ideone.com/v86876)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    vector<int> v1 = { 1,2,3,4,5,6,7,8,9,10 };
    vector<int> v2 = { 0,0,0,0,0,0,0,0,0,0 };

    auto p = remove(begin(v1), end(v1), 3);

    // 3의 배수를 제거
    auto p1 = remove_if(begin(v1), end(v1), [](int a) { return a % 3 == 0; });
}
```

---

## remove_copy 사용

* [Run This Code](https://ideone.com/pxWsk1)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    vector<int> v1 = { 1,2,3,4,5,6,7,8,9,10 };
    vector<int> v2 = { 0,0,0,0,0,0,0,0,0,0 };

    // 결과를 begin(v2)에 넣는다.
    remove_copy(begin(v1), end(v1), begin(v2), 3);
    // remove_copy_if도 동일하게 동작함.
}
```

---

## 추가로 알아 둘 것!

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    vector<int> v1 = { 1,2,3 };

    find( begin(v1), end(v1), 3);
    find( begin(v1), end(v1), foo);

    sort(begin(v1), end(v1));
    sort(begin(v1), end(v1), goo);  // 정렬 알고리즘을 변경가능. 단, sort_if가 아님을 기억하자.

    vector<int> v2 = { 0,0,0 };
    remove_copy(begin(v1), end(v1), begin(v2), 3);

    sort_copy(begin(v1), end(v1), begin(v2));
    // 이런 함수는 없다.
    // 큰 성능의 향상이 없기 때문이다.
}
```
