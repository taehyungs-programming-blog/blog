---
title: "C++ Template : type deduction"
permalink: cpp/template/Instantiation2/                # link 직접 지정
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
  - deduction
excerpt: ""
---

```cpp
template<typename T>
T square(T a)
{
  return a * a;
}

int main()
{
  square(3);    // 클래스도 type deduction이 가능할까?
  // 결론적으로 말하자면 C++17이전에는 불가능하다.
}
```

```cpp
template<typename T>
class Vector
{
  T* buff;
public:
  Vector() {}
  Vector(int sz, T initValue) {}
};

int main()
{
  // Vector<int> v1(10, 3);    // 10개를 3으로 초기화
  // C++ 17부터는 아래가 가능
  Vector v1(10, 3);
  
  Vector v3;    // 이게 될까 ??
  // 안됨. -> 가이드를 줘야 된다.
}
```

```cpp
template<typename T>
class Vector
{
  T* buff;
public:
  Vector() {}
  Vector(int sz, T initValue) {}
};

// 가이드는 이렇게 주자.
Vector()->Vector<int>;
// Vector를 그냥 쓰면 int로 추론해 주세요.(C++17 기능)

int main()
{
  Vector v3;
}
```

그래서 이게 뭐가 좋은데??

```cpp
#include <list>
using namespace std;

int main()
{
  // list<int> s = {1,2,3};  // 옛날 이야기
  list s = {1,2,3};         // C++17 - okay
}
```

```cpp
#include <list>
using namespace std;

template<typename T>
class Vector
{
  T* buff;
public:
  Vector() {}
  Vector(int sz, T initValue) {}
  
  template<typename C> Vector(C& c) {}
};

Vector()->Vector<int>;
Vector(C& c)->Vector<typename C::value_type>;    // 생성자로 C가 들어오면 C의 value_type으로 추론해 주세요.

int main()
{
  list s = {1,2,3};
  Vector v3(s);           // okay
}
```