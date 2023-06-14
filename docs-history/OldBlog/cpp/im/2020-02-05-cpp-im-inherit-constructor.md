---
title: "(C++) inherit constructor"
date: 2020-02-05 00:00:00 -0000
---

```cpp
// 생성자 상속
class Base
{
public:
  void foo(int a) {}
};

class Derived : public Base
{
public:
  // 함수의 이름이 foo라면?
  void foo() {}
};

int main()
{
  Derived d;
  d.foo(10);    // error - 누구를 호출하나? : 버그를 대비하는 것
  d.foo();      
}
```

```cpp
#include <iostream>
#include <string>
using namespace std;

class Base
{
  string name;
public:
  Base(string s) : name(s) {}
};

class Derived : public Base
{
public:
  Derviced(string s) : Base(s) {}
};

int main()
{
  Derived d("aa");    // Derived는 그냥 string을 Base에 던저주는일 밖에 안하는데... -> 간단하게 해볼까?
}
```

> 생성사 상속을 아래와 같이

```cpp
class Base
{
  string name;
public:
  Base(string s) : name(s) {}
};

class Derived : public Base
{
public:
  using Base::Base;   // 생성자를 상속해 달라
};
```