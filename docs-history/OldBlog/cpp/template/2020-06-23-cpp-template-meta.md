---
title: "C++ Template : template meta programming"
permalink: cpp/template/meta/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-23 00:00:00 -0000
last_modified_at: 2020-06-23 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - meta
excerpt: ""
---

```cpp
template<int N> struct factorial
{
  enum { value = N * factorial<N-1>::value };
  // static constexpr int value = N * factorial<N-1>::value;
  // 아래도 동일한 표현이다.
};

// 재귀 종료를 위해 특스화 된 문법
template<> struct factorial<1>
{
  enum { value = 1 };
  // static constexpr int value = 1;
};

int main()
{
  int n = factorial<5>::value;
  // 컴파일 시간에 이 값이 결정됨을 기억하자. -> template meta programming이라 한다.
  cout << n << endl;
}
```

---

## constexpr

```cpp
template<int N> struct check {};

constexpr int add(int a, int b)
{
  return a + b;
}

int main()
{
  int n = add(1, 2);
  check<add(1,2)> c;    // okay
  
  int n1 = 1;
  int n2 = 2;
  int nn = add(n1, n2);   // okay
  check<add(n1,n2)> c1;    // error
}
```