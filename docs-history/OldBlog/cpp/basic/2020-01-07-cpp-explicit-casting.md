---
title: "(C++) Explicit casting 명시적형변환"
permalink: cpp/explicit-casting/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-07 00:00:00 -0000
last_modified_at: 2020-09-24 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - explicit-casting
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* C : void* → 다른 타입의 포인터로 암시적 변환 허용
* C++ : void* → 다른 타입의 포인터로 암시적 변환 허용 안함.

```cpp
#include <iostream>
#include <cstdlib>

int main()
{
  // int* p1 = malloc(sizeof(int)*10);                    // compile error
  // int* p1 = (int*)malloc(sizeof(int)*10);              // ok
  int* p1 = static_cast<int*>(malloc(sizeof(int)*10));    // ok
  
  free(p1);
}
```

## C++ 명시적 형변환 종류

### static_cast 

가장 기본 적인 캐스팅 연산자<br>
정수와 실수 사이의 변환, 열거형과 정수 사이의 변환 등<br>

### reinterpret_cast

서로 다른 포인터 타입 끼리의 변환<br>

### const_cast

포인터와 참조형 상수성과 volatilie 속성을 제거하는데 사용<br>

### dynamic_cast

안전한 다운캐스팅(기반 클래스 포인터를 안전하게 파생 클래스 타입의 포인터로 캐스팅할때 사용)<br>
실행 시간 캐스팅 - 실행 시간 오버헤드 존재<br>

---

## Example

```cpp
void foo(int) {}
void foo(double) {}

int main(void)
{
  const int c = 10;
  double d = 3.4;
  
  int n = static_cast<int>(d);
  int* p1 = static_cast<int*>(malloc(100));   // malloc은 원래 void*를 리턴
  
  auto p2 = static_cast<void(*)(int)>(&foo);
  
  int* p3 = static_cast<int*>(&d);    // error
  int* p4 = static_cast<int*>(&c);    // error
}
```

```cpp
int main()
{
  int n = 10;
  // double* p1 = &n;
  double* p1 = reinterpret_cast<double*>(&n);
  
  // int* p2 = 10;
  int* p1 = reinterpret_cast<int*>(10);
  
  double d = reinterpret_cast<double>(n);   // error
  // static_cast로 충분한 것을 reinterpret_cast사용
}
```

```cpp
int main()
{
  const int c = 10;
  volatile int v = 20;
  
  int n = c;
  
  // int* p1 = &c;   // error
  int* p1 = const_cast<int*>(&c);
  // int* p2 = &v;   // error
  int* p2 = const_cast<int*>(&v);
  
  double* p3 = const_cast<double*>(&c);   // error
}
```