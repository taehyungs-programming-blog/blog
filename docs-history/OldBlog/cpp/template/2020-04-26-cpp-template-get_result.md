---
title: "C++ Template : get function result & argument"
permalink: cpp/template/get_result/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-26 00:00:00 -0000
last_modified_at: 2020-06-25 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - get function result
  - argument
excerpt: ""
---

```cpp
// 함수의 리턴 타입을 구해보자.
double hoo(short a, int b) { return 0; }

template<typename T> struct result_type
{
  // typedef T type;
  // 여기로 들어와서 type을 쓰려하면 에러를 출력하게 유도
};

template<typename R, typename A1, typename A2> 
struct result_type<R(A1, A2)>
{
  typedef R type;
};

template<typename T> void foo(T& t)
{
  // T : double(short, int)
  typename result_type<T>::type ret;
  // result_type<int>::type ret;
  cout << typeid(ret).name() << endl;
}

int main()
{
  foo(hoo);
}
```

```cpp
// 함수의 매개변수를 구해보자
double hoo(short a, int b) { return 0; }

template<typename T> struct argument_type
{
  // typedef T type;
  // 의도적 에러발생 유발
};

template<typename R, typename A1, typename A2, size_t N> 
struct argument_type<R(A1, A2), 0>
{
  typedef A1 type;
};

template<typename R, typename A1, typename A2, size_t N> 
struct argument_type<R(A1, A2), 1>
{
  typedef A2 type;
};

template<typename T> void foo(T& t)
{
  typename argument_type<T, 0>::type ret;
  cout << typeid(ret).name() << endl;
}

int main()
{
  foo(hoo);
}
```
