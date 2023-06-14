---
title: "(C++) 함수이름과 함수주소"
date: 2020-01-30 00:00:00 -0000
---

```cpp
#include <cstdio>

int square(int n)
{
  return n * n;
}

double square(double d)
{
  return d * d;
}

int main()
{
  printf("%p\n", &square);    // error - 어떤 square인지 알기 어려움
  printf("%p\n", static_cast<int(*)(int)>&square);    // ok
  
  auto p = &square;   // error - 역시 어떤 square인지 알수 없다.
  int(*f)(int) = &square;   // ok
}
```

> 그런데 `square`는 함수니 `square`와 `&square`는 동일한 표현아닌가?

```cpp
// 함수이름과 함수주소
#include <iostream>
#include <typeinfo>
using namespace std;

void foo(int a)
{
}

int main()
{
  void(*f1)(int) = &foo;    // ok!
  void(*f2)(int) = foo;     // ok, but! 함수 이름은 함수 주소로 암시적 형변환 된다.
  
  // 아래 두 개가 다르다는 것을 기억하자.
  typedef void(*PF)(int);   // 함수 포인터 타입
  typedef void F(int);      // 함수 타입..
  
  cout << typeid(&foo).name() << endl;  // void(*)(int)
  cout << typeid(foo).name() << endl;   // void(int)
}
```