---
title: "C++ Template : class template"
permalink: cpp/template/class/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-25 00:00:00 -0000
last_modified_at: 2020-06-17 00:00:00 -0000
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
template<typename T> class Complex
{
  T re, im;
public:
};

int main()
{
  Complex c1;     // T를 알수 없기에 error
  Complex<int> c2;// okay
}

void foo(Complex c);   // 이런 선언도 불가능하다
```

```cpp
template<typename T> class Complex
{
  T re, im;
public:
  void foo(Complex c);    // 요거는 가능하다
};
```

```cpp
template<typename T> class Complex
{
  T re, im;
public:
  // Complex(T a = 0, T b = 0) : re(a), im(b) {}
  // 0으로 초기화 될 수 없는 타입일 수 있기에 아래와 같은 코드가 더 좋은 코드라 할 수 있음
  Complex(T a = {}, T b = {}) : re(a), im(b) {}
  
  T getReal() const;
  // 외부에 함수를 구현
  
  static int cnt;
  // static 쓰기
  
  template<typename U> T func(const U& c);
  // 클래스 템플릿의 멤버함수 템플릿
};

// T Complex::getReal() const
// T가 뭐라고 알려줘야함
template<typename T> 
T Complex<T>::getReal() const
{
  return re;
}

template<typename T> 
int Complex<T>::cnt = 0;

template<typename T> template<typename U>
T Complex<T>::func(const U& c);

int main()
{
  
}
```