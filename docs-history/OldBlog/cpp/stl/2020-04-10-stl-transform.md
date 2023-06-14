---
title: "STL : transfrom"
permalink: cpp/stl/transfrom/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-10 00:00:00 -0000
last_modified_at: 2020-04-10 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* [Run This Code](https://ideone.com/3K3QIJ)

```cpp
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

int foo(int a) { return a+10; }

void eshow(int a) { cout << a << endl; }

int main() {
	list<int> s1 = { 1, 2, 3, 4, 5 };
    list<int> s2 = { 0, 0, 0, 0, 0 };

    transform( begin(s1), end(s1), begin(s2), foo);
    // begin(s1), end(s1)를 foo로 보내서 foo의 리턴을 begin(s2)에 넣어달라

    transform( begin(s1), end(s1), back_inserter(s2), foo);
    // 뒤쪽에 넣게 된다.

    for_each(begin(s1), end(s1), eshow);
    for_each(begin(s2), end(s2), eshow);
	return 0;
}
```

foo의 매개변수가 여러개 일때

* [Run This Code](https://ideone.com/nUcfua)

```cpp
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

int foo(int a, int b) { return a+b; }

void eshow(int a) { cout << a << endl; }

int main() {
	list<int> s1 = { 1, 2, 3, 4, 5 };
    list<int> s2 = { 1, 2, 3, 4, 5 };
    list<int> s3 = { 0, 0, 0, 0, 0 };

    transform( begin(s1), end(s1), begin(s2), begin(s3), foo);

    for_each(begin(s1), end(s1), eshow);
    return 0;
}
```

좀 더 좋은 코드는 ...

```cpp
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;

int foo(int a, int b) { return a+b; }

void eshow(int a) { cout << a << endl; }

int main() {
	list<int> s1 = { 1, 2, 3, 4, 5 };
    list<int> s2 = { 1, 2, 3, 4, 5 };
    list<int> s3 = { 0, 0, 0, 0, 0 };

    transform( begin(s1), end(s1), begin(s2), begin(s3), multiplies<int>());
    // transform( begin(s1), end(s1), begin(s2), begin(s3), [](int a, int b){return a+b;});

    for_each(begin(s1), end(s1), eshow);
    return 0;
}
```