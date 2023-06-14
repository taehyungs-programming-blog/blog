---
title: "C++ Template : 명시적/암시적 인스턴스화"
permalink: cpp/template/Instantiation1-1/                # link 직접 지정
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
  - template
  - Instantiation
  - 명시적
  - 암시적
excerpt: ""
---

```cpp
template<typename T> 
class Test
{
public:
  void foo() {}
  void goo() {}
};

template<typename T>
T square(T a)
{
  return a * a;
}

// 명시적 인스턴스화
template int square<int>(int);    // 기계어 코드를 미리 만들어 달라
// template int square<>(int);    // 동일표현

template void Test<int>::foo();   // 부분적으로 인스턴스화도 가능하다

int main()
{
  // 암시적 인스턴스화
  square(3);
  square<int>(3);
  
  Test<int> t1;
  t1.foo();     // goo는 인스턴스화 되지 않음.(주의) - lazy instantiation
}
```