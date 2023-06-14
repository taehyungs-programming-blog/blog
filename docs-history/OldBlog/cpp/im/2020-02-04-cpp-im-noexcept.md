---
title: "(C++) noexcept"
date: 2020-02-03 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

int foo()   // 예외가 있을수도 있고, 없을수도 있다.
int foo() throw(int)    // int형 예외가 있다는 의미
int foo() throw()       // 예외가 없다는 의미(C++98)
{
  
  return 0;
}

// C++11
void goo() noexcept(true)   // 예외가 없다.
void goo() noexcept         // 예외가 없다.
{
  throw 1;    // noexcept를 선언했는데 throw를 던지면?? -> 프로그램이 죽게된다.
}

int main()
{
  try
  {
    goo();
  }
  catch(...)
  {
    cout << "exception..." << endl;
  }
}
```

> 아니 근데 noexcept를 왜 쓰는건데??

```cpp
// 안쓴다고 가정해보자
void goo()
{
  throw 1;
}

int main()
{
  goo();
  // 예외가 없다고 알려주면 조금더 좋은 코드를 컴파일러가 만들어 준다고 한다...
}
```

```cpp
#include <iostream>
using namespace std;

void algo1()
{
  // 빠르지만 예외 가능성이 있다.
}

void algo2()
{
  // 느리지만 예외가 나오지 않는다.
}

int main()
{
  // algo1 에 예외가 있는지 확인
  bool b1 = noexcept(algo1());      // 0
  bool b2 = noexcept(algo2());      // 1
}
```