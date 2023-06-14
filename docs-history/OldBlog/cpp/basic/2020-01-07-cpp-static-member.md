---
title: "(C++) static memeber"
#permalink: cpp/basic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-07 00:00:00 -0000
last_modified_at: 2020-03-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## 정적 멤버 데이터(static member data)

* static이 붙은 멤버 데이터
* 모든 객체가 공유할 수 있다.
* 클래스에서 내부 선언을 만들고
* 외부에 정의를 만들어야 한다.

```cpp
#include <iostream>

class Car
{
  int speed;
  int color;
  static int cnt;     // 내부의 선언
public:
  Car() { ++cnt; }
  ~Car() { --cnt; }
};

// cpp에 두는것이 원칙이다.!
int Car::cnt = 0;     // 외부의 정의

int main()
{
  Car c1;
  Car c2;
  Car c3;
}
```

---

## another example

```cpp
class Test
{
public:
  int data = 0;
  
  static int sdata1;
  // static int sdata1 = 0; // 선언이 되기에 error 
  
  static const      int       sdata2 = 0; // ok and 외부에 선언을 하지 않아도 됨.
  static const      double    sdata3 = 0; // error 정수는 되지만 실수는 안된다.
  static constexpr  int       sdata4 = 0; // ok - constexpr은 실수/정수 모두 가능
  static constexpr  double    sdata5 = 0; // ok
  
  inline static int sdata6 = 0; // C++17부터는 가능
};

int Test::sdata1 = 0;
```

---

## 정적 멤버 함수(static member function)

```cpp
class Car
{
  int speed;
  static int cnt;
public:
  Car() { ++cnt; }
  ~Car() { --cnt; }
  
  static int getCount() { return cnt; } // 정적 멤버 함수
};

int Car::cnt = 0;

int main()
{
  Car c1, c2, c3;
  c1.getCount();    // 정적 멤버 함수를 통해서 객체를 만들지 않고도 getCount를 호출하고 싶다.

  Car::getCount();  // 객체 없이 호출 가능.
}
```