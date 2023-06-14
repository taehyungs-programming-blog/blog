---
title: "(C++) perfect forwarding 개념"
date: 2020-02-13 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

void goo(int& a) { cout << "goo" << endl; a = 30; }
void foo(int a) { cout << "foo" << endl; }

int main()
{
  foo(5);   // 이 함수에 걸리는 시간을 측정하고 싶다면?
}
```

```cpp
chronometry(&foo, 5);   // foo(5)
// 이걸 어떻게 만들까?
```

```cpp
template<typename F, typename A>
void chronometry(F f, A arg)    // 그런데 이렇게 만들면 A를 값으로 받기에 완벽한 복사라 할 수 없다.
{
  f(arg);
}

int main()
{
  // 예를 들어
  int n = 0;
  goo(n);   // n은 30으로 변경이 되나
  chronometry(&goo, n); // n이 30으로 변경되지 못한다.
}

// 어떻게 해결해야 하나?
```

```cpp
template<typename F, typename A>
void chronometry(F f, A& arg)    // 참조로 받자?? -> 이러면 rvalue를 받을 수 없다.
{
  f(arg);
}

int main()
{
  chronometry(&foo, 5);   // error
}
// 난감하구만 ...
// 여기서 완벽한 전달(perfect forwarding)의 개념이 나온다.
// 래퍼함수가 인자를 받아서 원본 함수에게 완벽하게 전달하고자 하는 개념이다.
// 이걸 만들어 보고자 한다.!
```