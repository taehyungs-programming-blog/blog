---
title: "STL algorithm"
permalink: cpp/stl/algorithm/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-01 00:00:00 -0000
last_modified_at: 2020-04-01 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <cstring>
using namespace std;

int main()
{
    char s[] = "abcdefg";
    char * p = strchr(s, 'c');      // 문자열의 시작부터 null문자열까지 검색해달라

    if(p == 0)
        cout << "fail" << endl;
    else
        cout << "success" << *p << endl;
}
```

문자열 검색에 조건을 넣고싶다<br>

```cpp
char* estrchr(char* first, char* last, char value)
{
    while(first != last && *first != value)
        ++first;
    return first == last ? 0 : first;
}

char * p = estrchr(s, s+4, 'c');        // 단, half open range : 시작은 검색에 넣지만 마지막은 검색에 넣지 않음.
```

모든 타입의 배열을 검색하게 해보자.

```cpp
template<typename T>
T* efind(T* first, T* last, T value)
{
    while(first != last && *first != value)
        ++first;
    return first == last ? 0 : first;
}
```

문제는 `double * p = estrchr(s, s+4, 5);`한 경우에 컴파일러는 5를 int로 보게된다.

```cpp
template<typename T1, typename T2>
T1 efind(T1 first, T1 last, T2 value)
{
    while(first != last && *first != value)
        ++first;
    return first == last ? 0 : first;
}
```

null 리턴이 문제를 이르킬 수 있으니 마지막 수를 리턴하게 만들어 보자.

```cpp
template<typename T1, typename T2>
T1 efind(T1 first, T1 last, T2 value)
{
    while(first != last && *first != value)
        ++first;
    return first;
}

int  main()
{
    double x[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double* p = efind(x, x+10, 5);

    if(p == x+10)
        cout << "fail" << endl;
}
```

STL에서 지원하는 find를 실제로 보자.

```cpp
#include <iostream>
#include <algorithm>
using namespace std;

int main()
{
    double x[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double* p = find(x, x+10, 5);

    if(p == x+10)
        cout << "fail" << endl;
}
```