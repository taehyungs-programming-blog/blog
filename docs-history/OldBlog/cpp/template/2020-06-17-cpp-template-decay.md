---
title: "C++ Template : argument decay"
permalink: cpp/template/decay/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-16 00:00:00 -0000
last_modified_at: 2020-06-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
category:
  - decay
excerpt: ""
---

```cpp
template<typename T>
void foo(T a)     // 배열을 받을 순 없다.
{
}

template<typename T>
void goo(T& a)    // 결론은 참조로 받으면 배열을 받을 수 있다.
{
}

int main()
{
  int x[3] = {1,2,3};
  
  int y[3] = x;   // error
  int* p = x;     // ok
  int(&r)[3] = x; // ok
  
  foo(x);   
  // T : int[3] - error
  // T : int* - okay
  goo(x);
  // T : int[3] - okay
}
```

```cpp
// 주의사항
template<typename T> void foo(T a, T b)
{
}
template<typename T> void goo(T& a, T& b)
{
}

int main()
{
  foo("orange", "apple");   // okay
  goo("orange", "apple");   // error
  // orange와 apple은 배열의 크기가 다른 배열이기에 같은 T로 초기화 불가능하다
}
```