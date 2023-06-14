---
title: "(C++) rvalue reference"
date: 2020-02-10 00:00:00 -0000
---

```cpp
int main()
{
  int n = 10;
  
  int& r1 = n;
  int& r2 = 10;   // error
  
  const int& r3 = n;
  const int& r4 = 10;   // ok - 규칙 1. not const reference는 lvalue만 참조
  // 규칙 2. const reference는 lvalue, rvalue 모두 참조 가능.
  // 단, 이런식으로 가리키면 상수성을 갖게됨.
  
  // 규칙 3. rvalue reference는 rvalue만 가리킬수 있다.
  int&& r5 = n;   // error
  int&& r6 = 10;  // ok
}
```

> 아니 rvalue reference는 어디쓰나? move, 퍼펙트 포워딩에 사용됨 → 나중에 살명

```cpp
#include <iostream>
using namespace std;

void foo(int& a)    { cout << "int&" << endl; }       // 1
void foo(const int& a) { cout << "const int&" << endl; }    // 2
void foo(int&& a)   { cout << "int&&" << endl; }      // 3

int main()
{
  int n = 10;
  
  foo(n);   // 1번 -> 2번 순
  foo(10);  // 3번 -> 2번 순
  
  int& r1 = n;
  foo(r1);    // 1번 -> 2번 순
  
  int&& r2 = 10;  // 10은 rvalue r2는 이름을 갖기에 r2는 lvalue가 된다.
  foo(r2);    // 1번
  
  foo(static_cast<int&&>(r2));    // 3번
} 
```