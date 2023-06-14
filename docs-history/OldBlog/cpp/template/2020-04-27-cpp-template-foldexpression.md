---
title: "C++ Template : fold expression"
permalink: cpp/template/foldexpression/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-27 00:00:00 -0000
last_modified_at: 2020-04-27 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
using namespace std;

template<typename ... Types>
int foo(Types ... args)
{
    int n = (args + ...);     // fold expression - C++17부터 지원한다 
                              // 1+(2+(3+4))
    return 0;
}

int main()
{
    int n = foo(1,2,3,4);   // args : 1,2,3,4
    cout << n << endl;
}
```

fold expression은 네 가지 형태를 지원한다.
* (args op ...) -> E1 op (E2 op (E3 op E4))
* (... op args) -> ((E1 op E2) op E3) op E4
* (args op ... op init) -> E1 op (E2 op (E3 op (E4 op init)))
* (init op ... op args) -> (((init op E1) op E2) op E3) op E4

```cpp
#include <iostream>
using namespace std;

template<typename ... Types>
void foo(Types ... args)
{
    (cout << ... << args);      // (init op ... op args) -> (((init op E1) op E2) op E3) op E4
    // (( cout << 1) << 2) << 3
}

int main()
{
    foo(1,2,3);
}
```

```cpp
#include <iostream>
#include <vector>
using namespace std;

vector<int> v;

template<typename ... Types>
void foo(Types ... args)
{
    (args, ...); // 1, (2, 3)
    (v.push_back(args), ...);
    // v.push_back(1), (v.push_back(2), v.push_back(3));
}

int main()
{
    foo(1,2,3);
}
```