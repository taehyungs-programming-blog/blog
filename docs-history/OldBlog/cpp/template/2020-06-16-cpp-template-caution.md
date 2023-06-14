---
title: "C++ Template : 응용) template 사용시 주의사항"
permalink: cpp/template/caution/                # link 직접 지정
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
  - caution
excerpt: ""
---

* 우선 template은 선언과 구현부를 h/cpp로 나눠서는 안된다

```cpp
#include <iostream>
using namespace std;

template<typename T> T square(T a)
{
  return a * a;
}

int main()
{
  printf("%p\n", &square);    // 이렇게 하면 주소가 나올까?
  // 아직은 주소가 없기에 출력이 안된다.
  
  printf("%p\n", &square<int>); // 주소가 출력이 된다.
  
  // 함수 포인터를 뽑을 수도 있다
  printf("%p\n", static_cast<int(*)(int)>(&square));    // ok
}
```