---
title: "STL : algorithm"
permalink: cpp/stl/algorithm2/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-10 00:00:00 -0000
last_modified_at: 2020-04-10 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
using namespace std

int main()
{
    vector<int> v = { 1, 2, 3, 1, 2, 3, 1, 2, 3, 1 };

    auto p = find(begin(v), end(v) ,3);

    return 0;
}
```

* [Run This Code](https://ideone.com/7esuQz)

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

int main()
{
    vector<int> v = { 1, 2, 3, 1, 2, 3, 1, 2, 3, 1 };

    // 알고리즘은 컨테이너를 알지 못한다.
    auto p = remove(begin(v), end(v), 3);

    show(v);        // 1, 2, 1, 2, 1, 2, 3, 1   // 3이 제거가 되지 않음.
    // vector의 경우 메모리를 제거하지않고 3이 나오면 메모리를 한 칸씩 앞으로 당긴다.
    // 마지막에 남은 3이 출력되어 버린것!

    v.erase(p, end(v));     // 남아 있는 값을 그냥 지워버리면 된다.

    return 0;
}
```

---

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

int main()
{
    list<int> v = { 1, 2, 3, 1, 2, 3, 1, 2, 3, 1 };

    // list인데 아래의 remove가 최선일까?
    // 위 예제에서 보듯 remove는 앞으로 한칸씩 당기는 일을한다.
    auto p = remove(begin(v), end(v), 3);
    v.erase(p, end(v));

    v.remove(3);    // list 전용 remove를 쓰자

    return 0;
}
```