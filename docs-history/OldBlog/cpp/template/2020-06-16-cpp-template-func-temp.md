---
title: "C++ Template : 응용) 함수 템플릿 만들기"
permalink: cpp/template/func-template/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-21 00:00:00 -0000
last_modified_at: 2020-04-21 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
category:
  - template
  - func-template
excerpt: ""
---

```cpp
template<typename T> void foo(T a) {}

template<typename T, typename U> struct pair
{
  T first;
  U second;
  pair(const T& a, const U& b) : first(a), second(b) {}
};

int main()
{
  pair<int, double> p(1, 3.4);
  foo(p);
  
  foo(pair<int, double>(1, 3.4));
  // C++17 이전에는 <int, double>을 반드시 넣어줘야 컴파일러가 타입 추론이 가능했다.
  // 참고로 설명하자면 class(struct)에 대한 type deduction은 C++17이전에는 불가능.
  // 그렇다면 C++17이전에는 매번 <int, double> 타입을 적어주는게 싫은데 ...
  // -> make_pair를 만들어 보자.
}
```

```cpp
template<typename T> void foo(T a) {}

template<typename T, typename U> struct pair
{
  T first;
  U second;
  pair(const T& a, const U& b) : first(a), second(b) {}
};

template<typename T, typename U>
pair<T, U> make_pair(const T& a, const U& b)
{
  return pair<T, U>(a, b);
}

int main()
{
  pair<int, double> p(1, 3.4);
  foo(p);
  
  // foo(pair<int, double>(1, 3.4));
  foo(make_pair(1, 3.4));           // 이렇게 선언이 가능.
  
  // C++에서 사용중인 예제를 보자면.
  // foo(tuple<int, double, int>(1, 3.4, 1));
  foo(make_tuple(1, 3.4, 1));
  
  // 물론 C++17이후에는 사용빈도가 줄어듦.
}
```

```cpp
template<typename T> struct identity
{
  typedef T type;
};

template<typename T> void foo(T a) {}
template<typename T> void goo(typename identity<T>::type a) {}

int main()
{
  identity<int>::type n;    // type은 선언된 타입이 튀어나온다.
  // int로 선언되었는데 type을 호출 시 int가 튀어 나옴.
  
  foo(3);         // okay
  foo<int>(3);    // okay
  
  goo(3);         // error - identity<T>에서 T를 추론할 수 없음.
  goo<int>(3);    // okay
}
```