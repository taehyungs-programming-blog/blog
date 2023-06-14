---
title: "C++ Template : 개념"
permalink: cpp/template/basic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-21 00:00:00 -0000
last_modified_at: 2020-06-16 00:00:00 -0000
header:
  teaser: /file/image/cpp-page-teaser.gif
sidebar:
  title: "C++"
  nav: cpp
tag:
  - C++
category:
  - template
  - 개념
excerpt: ""
---

```cpp
int square(int a)
{
    return a * a;
}

double square(double d)
{
    return d * d;
}

int main()
{
    // 함수 오버로딩을 이용
    // 이 square를 매번 개발자가 만들어 줘야할까?
    square(3);
    square(3.3);
}
```

* [Run This Code](https://ideone.com/VCrC61)

```cpp
#define MAKE_SQUARE(T)  \
T square(T a)           \
{                       \
    return a * a;       \
}                       

MAKE_SQUARE(int)
MAKE_SQUARE(double)

int main()
{
    square(3);
    square(3.3);
}
```

이걸 좀 더 편하게 만들어 볼까?

* [Run This Code](https://ideone.com/SfUMlR)

```cpp
template<typename T>
T square(T a)           
{                       
    return a * a;       
}  

int main()
{
    square<int>(3);
    square<double>(3.3);

    square(3);      // ok - 컴파일러는 3이 int란걸알기에 가능
    square(3.3);    // ok!
}
```

* template은 진짜 함수가 아니라 함수를 만들어 내는 틀이다.

```cpp
//template<typename T>
template<class T>       //typename, class 모두 똑같은 기능을 한다.
T square(T a)           
{                       
    return a * a;       
}  
```

---

## Template의 필요성(언제쓰나?)

```cpp
class Complex
{
    int re, im;
public:
    Complex(double r, double i) : re(r), im(i) {}
    // 받기는 double로 받는데 실제로 int네?? -> 이런실수가 나올 수 있다.
};
```

```cpp
template<typename T>
class Complex
{
    T re, im;
public:
    Complex(T r, T i) : re(r), im(i) {}
};
```