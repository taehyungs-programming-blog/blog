---
title: "C++ Template : 함수 인자타입구하기"
permalink: cpp/template/funcargument/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-27 00:00:00 -0000
last_modified_at: 2020-06-30 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - 함수 인자타입구하기
excerpt: ""
---

```cpp
double hoo(short a, int b) { return 0; }

template<size_t N, typename T> struct argument_type
{
  typedef T type;
};

template<typename R, typename A1, typename ...Types>    // 가변인자를 처리
struct argument_type<0, R(A1, Types...)>
{
  typedef A1 type;
};

template<typename T> void foo(const T& t)
{
  // T : double(short, int)
  typename argument_type<0, t>::type ret;
  // argument_type<0, t> -> 0이 아니라 다른 건 못오나?
  
  cout << typeid(ret).name() << endl;
}

int main()
{
  foo(hoo);
}
```

```cpp
template<typename R, typename A1, typename ...Types>
struct argument_type<0, R(A1, Types...)>
{
  typedef A1 type;
};

template<size_t N, typename R, typename A1, typename ...Types>
struct argument_type<N, R(A1, Types...)>
{
  typedef typename argument_type<N-1, R(Types...)>::type type;
};

template<typename T> void foo(const T& t)
{
  // T : double(short, int)
  typename argument_type<2, t>::type ret;
  
  cout << typeid(ret).name() << endl;
}
```