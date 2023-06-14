---
title: "(C++) reference collapse"
date: 2020-02-10 00:00:00 -0000
---

```cpp
using LREF = int&;    // typedef int& LREF;
using RREF = int&&;   // typedef int&& RREF;

int main()
{
  int n = 10;
  
  LREF r1 = n;
  RREF r2 = 10;
  
  // 아래와 같은 경우가 reference collapse이다. -> &가 떨어져서 연속으로 나올경우 컴파일러가 어떻게 처리하는지 기준을 알려준다.
  LREF& r3 = n; // int& & -> int&
  RREF& r4 = n; // int&& & -> int&
  LREF&& r5 =  n; // int& && -> int&
  RREF&& r6 = 10; // int&& && -> int&&
  
  int& && r7;   // 사용자가 직접쓰면 컴파일 에러가 발생한다. (위 처럼 실수를 방지하는 용도이다.)
}
```