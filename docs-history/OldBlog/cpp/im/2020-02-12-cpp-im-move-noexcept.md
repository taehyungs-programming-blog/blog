---
title: "(C++) move & noexcept"
date: 2020-02-11 00:00:00 -0000
---

```cpp
#include <iostream>
#include <vector>
#include <type_traits>
#include "Test.h"
using namespace std;

int main()
{
  Test* p1 = new Test[2];
  Test* p2 = new Test[4];
  
  for(int i = 0; i < 2; i++)
    p2[i] = move(p1[i]);    // move를 진행 중 예외가 발생하면 큰 문제가 발생할 수 있음 -> STL에서 noexcept를 강제하는 이유
}
```

```cpp
Test t1;
Test t2 = t1;   // copy
Test t3 = move(t2); // move

bool b = is_nothrow_move_constructible<Test>::value;    // Test의 move생성자에 예외가 있을 시 true

cout << b << endl;    // true

Test t4 = move_if_noexcept(t1);   // 예외가 없다면 move 생성자 호출
```

* 버퍼를 복사 할 시 무조건 move쓴다고 move로 동작하지는 않는다.
* 예외를 던지는 지 확인하고 move생성자에 noexcept을 붙이는 것을 잊지말자