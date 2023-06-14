---
title: "C++ Template : SFINAE"
permalink: cpp/template/SFINAE/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-30 00:00:00 -0000
last_modified_at: 2020-04-30 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

우선 아래의 순서를 명확히 기억하자

```cpp
#include <iostream>
using namespace std;

template<typename T>
void foo(T t) { cout << "T" << endl; }
void foo(itn t) { cout << "int" << endl; }
void foo(...) { cout << "..." << endl; }

int main()
{
    foo(3);     // int - exactly matching
}
```

```cpp
#include <iostream>
using namespace std;

template<typename T>
void foo(T t) { cout << "T" << endl; }
// void foo(itn t) { cout << "int" << endl; }
void foo(...) { cout << "..." << endl; }

int main()
{
    foo(3);     // T
}
```

```cpp
#include <iostream>
using namespace std;

template<typename T>
// void foo(T t) { cout << "T" << endl; }
// void foo(itn t) { cout << "int" << endl; }
void foo(...) { cout << "..." << endl; }

int main()
{
    foo(3);     // ...
}
```

```cpp
template<typename T>
void foo(T t) { cout << "T" << endl; }

void foo(...) { cout << "..." << endl; }

int main()
{
    foo(3);     // T
}
```

```cpp
template<typename T>
int foo(T t) { cout << "T" << endl; return 0; }

void foo(...) { cout << "..." << endl; }

int main()
{
    foo(3);     // T
}
```

```cpp
template<typename T>
typename T::type foo(T t) // int::type foo(int t) - 잘못된 표현이다.
{ 
    cout << "T" << endl; 
    return 0; 
}

void foo(...) { cout << "..." << endl; }

int main()
{
    foo(3);     // 에러가 나올것인가?
    // 에러가 나오지 않고 ...가 호출된다. -> SFINAE
}
```

* Subtitution Faiure Is Not An Error