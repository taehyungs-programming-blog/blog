---
title: "C++ Template : type 출력하기"
permalink: cpp/template/typeid/                # link 직접 지정
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
  - typeid
excerpt: ""
---

```cpp
#include <iostream>
using namespace std;

template<typename T> void foo(const T a)
{
  cout << typeid(a).name() << endl;
  cout << typeid(T).name() << endl;
  // 타입을 출력해 준다. 단, const, volatile, reference는 조사가 불가능
  // boost를 사용해야 한다.
}

int main()
{
  foo<int>(3);
  
  foo(3);     // T : int, a : const int
  foo(3.3);   // T: double, a : const double
}
```

```cpp
#include <iostream>
#include <boost\type_index.hpp>
using namespace std;
using namespace boost::typeindex;

template<typename T> void foo(const T a)
{
  cout << type_id_with_cvr<T>().pretty_name() << endl;
  cout << type_id_with_cvr<decltype(a)>().pretty_name() << endl;
  // const, volatile, reference까지 출력이 가능.
}

int main()
{
  foo(3);     // T : int, a : const int
  foo(3.3);   // T: double, a : const double
}
```