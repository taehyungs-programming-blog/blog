---
title: "C++ Template : 복사 생성자"
permalink: cpp/template/copycon/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-25 00:00:00 -0000
last_modified_at: 2020-04-25 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
category:
  - 복사 생성자
excerpt: ""
---

```cpp
template<typename T> class Complex
{
  T re, im;
public:
  Complex(T r = {}, T i = {}) : re(r), im(i) {}
};

int main()
{
  Complex<int> c1(1, 1);      // okay
  Complex<int> c2 = c1;       // okay
  
  Complex<double> c3 = c1;    // 타입이 다르기에 error
  // 복사 생성자를 만들어서 에러가 발생하지 않게 만들어 보자.
}
```

```cpp
template<typename T> class Complex
{
  T re, im;
public:
  Complex(T r = {}, T i = {}) : re(r), im(i) {}
  
  // 일반적 복사 생성자
  Complex(const Complex<T>& c) {}
  
  // 자신과 다른 타입의 복사 생성자
  template<typename U>
  Complex(const Complex<U>& c);
  
  template<typename> friend class Complex;    // 상호간 접근이 가능하게 만들어 줘야한다.
};

tempate<typename T> template<typename<U>
Complex<T>::Complex(const Complex<U>& c) : re(c.re), im(c.im)
{
}
```
