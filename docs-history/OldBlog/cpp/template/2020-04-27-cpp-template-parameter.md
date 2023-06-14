---
title: "C++ Template : parameter"
permalink: cpp/template/parameter/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-27 00:00:00 -0000
last_modified_at: 2020-06-18 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - parameter
excerpt: ""
---

```cpp
// template의 파라미터로
// type, 값, template을 받을 수 있음.
template<typename T, int N>
struct Stack
{
  T buff[N];
};

int main()
{
  Stack<int, 10> s;
}
```

```cpp
// type을 파라미터로 받기
template<typename T> class List
{
};

int main()
{
  list<int> s1;
}
```

```cpp
// 값을 파라미터로 받기
template<int N> class Test1 {};

enum Color { red = 1, green = 2 };
template<Color> class Test2 {};

template<int*> class Test3 {};

int x = 0;

template<int(*)(void)> class Test4 {};

int main()
{
  int n = 10;
  Test1<10> t1;   // okay
  Test1<n> t2;    // error - 컴파일 타임에 결정이 되어야 함.
  
  // enum도 가능
  Test2<red> t3;
  
  // 포인터도 가능
  Test3<&n> t4;   // error - stack(지역변수)의 주소는 에러
  Test3<&x> t5;   // ok
  
  // 함수포인터도 가능
  Test4<&main> t6;    // ok
}
```

```cpp
template<auto N> struct Test {};

int x = 0;

int main()
{
  Test<10> t1;      // okay
  Test<&x> t2;      // okay
  Test<&main> t3;   // okay
}
```

```cpp
// template을 받기
template<typename T> class list {};

template<typename T, template<typename> class C> class stack
{
  // C c;   // error
  C<T> c;   // okay - 템플릿이기에 타입을 지정해 줘야함.
};

int main()
{
  list s1;      // error - list는 template
  list<int> s2; // okay - list<int> 타입
  
  stack<int, list>  s3;   // okay - 템플릿 틀을 보내야함.
}
```

```cpp
// 디폴트 값 지정
template<typename T = int, int N = 10> 
struct Stack
{
};

int main()
{
  Stack<int, 10> s1;  // okay
  Stack<int> s2;      // okay
  
  Stack<> s3;         // 모든 인자를 디폴트 적용
}
```