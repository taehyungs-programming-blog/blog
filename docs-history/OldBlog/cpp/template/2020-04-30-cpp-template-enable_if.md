---
title: "C++ Template : enable_if"
permalink: cpp/template/enable_if/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-30 00:00:00 -0000
last_modified_at: 2020-04-30 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
template<bool b, typename T = void> struct enable_if
{
};

template<typename T> struct enable_if<true, T>
{
    typedef T type;
};

int main()
{
    enable_if<ture, int>::type t0;      // int
    enable_if<ture>::type t1;           // void
    enable_if<false, int> t2;           // error - type이 없다
    enable_if<fale>::type t3;           // error - type이 없다
}
```

enable_if 활용

```cpp
/*
// 정수일때만 함수 코드를 생성하고 싶다.
template<typename T> void foo(T a)
{
    static_assert(is_integeral<T>::value, "error");
}
*/
template<typename T> 
typename enable_if<is_integral<T>::value>::type
foo(T a)
{
    static_assert(is_integeral<T>::value, "error");
}

// 정수가 아니면 아래 함수에서 처리하고 싶다면?
void foo(...)
{
    cout << "not integer" << endl;
}

int main()
{
    foo(3.4);
}
```

---

## enable_if 위치

```cpp
// 정수가 아니면 에러를 발생하게 하고 싶다.
/*
template<typename T> void foo(T a)
{
    static_assert(is_integral<T>::value, "error");
}
*/
template<typename T> 
typename enable_if<is_integral<T>::value>::type
foo(T a)
{
}

template<typename T> 
void foo(T a, typename enable_if<is_integral<T>::value>::type* = nullptr)
{
}

template<typename T, 
        typename enable_if<is_integral<T>::value>::type* = nullptr> 
void foo(T a)
{
}

// 정수가 아니면 사용하지 않게 하고 싶다.

void foo(...){}

int main()
{
    foo(3.4);
}
```