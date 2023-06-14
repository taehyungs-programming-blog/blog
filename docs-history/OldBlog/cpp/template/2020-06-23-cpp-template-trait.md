---
title: "C++ Template : type_traits & is_pointer"
permalink: cpp/template/trait/                # link 직접 지정
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
  - type_traits
  - is_pointer
excerpt: ""
---

```cpp
template<typename T> void printv(T v)
{
  // 이걸 하고 싶다
  // if(T is Pointer)
  // cout 메모리 주소
  cout << v << endl;
}

int main()
{
  int n = 3;
  double d = 3.4;
  
  printv(n);
  printv(d);
  printv(&n);
}
```

```cpp
template<typename T> void foo(T v)
{
  if(?)   // 여기 뭘 넣어야 할까?
    cout << "pointer" << endl;
  else
    cout << "not pointer" << endl;
}

int main()
{
  int n = 3;
  foo(n);
  foo(&n);
}
```

```cpp
template<typename T> struct xis_pointer
{
  enum { value = false };
};

// 포인터 타입에 대해서 부분 특수화
template<typename T> struct xis_pointer<T*>
{
  enum { value = true };
};

template<typename T> void foo(T v)
{
  if(xis_pointer<T>::value)   // 함수처럼 보이고 컴파일 시간에 돌아간다고 해서 메타 함수라 한다.
    cout << "pointer" << endl;
  else
    cout << "not pointer" << endl;
}
```