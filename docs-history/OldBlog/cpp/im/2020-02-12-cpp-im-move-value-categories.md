---
title: "(C++) value categories"
date: 2020-02-12 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

void f1(int a) {}
void f2(int& a) {}
void f3(int&& a) {}

int main()
{
  int n = 10;
  
  f1(n);
  f2(n);
  f3(static_cast<int&&>(n));    
  f3(move(n));    // 위와 동일한 표현
}
```

```cpp
// 리턴 타입에 따른 리턴 방법
int x = 0;
int f4() { return x; }
int& f5() { return x; }
int&& f6() { return static_cast<int&&>(x); }
int&& f6() { return move(x); }
```

* 지역 변수는 참조로 리턴 할 수 없다.
* 함수 리턴 타입의 종류에 따라서 리턴 하는 방법을 기억하자.

---

```cpp
#include <iostream>
usinag namespace std;

int x = 0;
int f1() { return x; }
int& f2() { return x; }
int&& f3() { return move(x); }

int main()
{
  f1() = 10;  // error
  f2() = 20;  // ok
  f3() = 30;  // error, rvalue 리턴이기에 안됨
}
```

---

```cpp
// 조금 더 심화해보자.
struct Base
{
  virtual void foo() { cout << "B::foo" << endl; }
};
struct Derived : public Base
{
  virtual void foo() { cout << "D::foo" << endl; }
};

Derived d;
Base f1() { return d; }
Base& f2() { return d; }
Base&& f3() { return move(d); }

int main()
{
  Base b1 = f1();   // 임시객체, move
  Base b2 = f2();   // copy
  Base b3 = f3();   // move
  
  f1().foo();     // B::foo, 비다형적
  f2().foo();     // D::foo, 다형적
  f3().foo();     // D::foo, 다형적
  
  int n = 10;
  n = 20;   // n : lvalue, 20 : prvalue
  int n3 = move(n);   // xvalue - rvalue를 리턴하는 모든 표현식을 의미
}
```

*참고로 C++11 이후부터는 단순 r, l value로 나누는 것이 아니라 총 다섯개의 value로 나뉘게 된다.

* glvalue
    * lvalue - copy, polymorphic, has id
    * xvalue - move, polymorphic, has id
* rvalue
    * xvalue - move, polymorphic, has id
    * prvalue - move non-polymorphic, no id

> cpp reference에 정확한 설명이 있음!