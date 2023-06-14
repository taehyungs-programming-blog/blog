---
title: "C++ Template : pack extension 하는 다른 방법"
permalink: cpp/template/packexpansion/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-27 00:00:00 -0000
last_modified_at: 2020-06-28 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - pack expansion
excerpt: ""
---

```cpp
#include <iostream>
#include <tuple>
using namespace std;

template<typename ... Types> void foo(Types ... args)
{
  int x[] = { args... };  // 그런데 타입이 다르다면?
  tuple<Types...> tp(args...);  // tuple을 쓰자
  
  cout << get<0>(tp) << endl;
  cout << get<1>(tp) << endl;
  cout << get<2>(tp) << endl;
  
  // 다른 방버은 없나?
}

int main()
{
  foo(1, 3.4, "AA");
}
```

```cpp
void foo() {} // 재귀의 종료를 위해

template<typename T, typename ... Types> 
void foo(T value, Types ... args)
{
  cout << value << endl;

  // 재귀적 호출(but 코드상으론 여러 함수가 생성됨을 기억)
  foo(args...);
}

int main()
{
  foo(1, 3.4, "AA");
}
```

---

## fold expression

```cpp
#include <iostream>
using namespace std;

template<typename ... Types>
int foo(Types ... args)
{
  int x[] = { args... };  // pack expansion
  
  int n = ( args + ... );   // fold expression -> 다른 기능도 있지만 필요하면 찾아서 쓰자.
  // 1+(2+(3+4))
  
  return 0;
}

int main()
{
  int n = foo(1, 2, 3, 4);
}
```

---

## 활용하기

```cpp
template<typename ... Types>
void foo(Types ... args)
{
  ( cout << ... << args );
  // ((( cout << 1) << 2 ) << 3);
}

int main()
{
  cout << 1 << 2 << 3;
  // (((cout << 1) << 2) << 3);
  
  foo(1, 2, 3);
}
```

```cpp
vector<int> v;

template<typename ... Types>
void foo(Types .. args)
{
  ( args, ... );  
  // 1, (2, 3)
  
  ( v.push_back(args), ... );
  // v.push_back(1), (v.push_back(2), v.push_back(3));
  
  for(auto n : v)
    cout << n << endl;
}

int main()
{
  foo(1, 2, 3);
}
```