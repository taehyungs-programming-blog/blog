---
title: "(C++) decltype"
date: 2020-02-06 00:00:00 -0000
---

```cpp
int main()
{
  int n = 0;
  int* p = &n;
  
  decltype(n) d1;     // int
  decltype(p) d2;     // int*, 명확하게 자료형이 나오면 그 자료형 대로 판단
  
  decltype(*p) d3;     // int&, decltype(수식) : 수식이 lvalue라면 참조, 아니면 값 타입
  decltype((n)) d4;    // int&, 변수에 연산자("()")가 붙어있어 lvalue라 판단
  
  decltype(n+n) d5;     // int, n+n = 10 -> error
  decltype(++n) d6;     // int&, ++n = 10 -> ok
  decltype(n++) d7;     // int, n++ = 10 -> error
  
  int x[3] = {1,2,3};
  
  decltype(x[0]) d8;    // int&, x[0] = 10 -> ok
  auto a1 = x[0];       // int
  // 같은 x[0]라도 decltype과 auto의 차이를 알자
}
```

```cpp
int x = 10;

int& foo(int a, int b)
{
  return x;
}

int main()
{
  auto ret1 = foo(1, 2);      // int, auto는 참조형 타입을 제거하고 받음.
  decltype(foo(1,2))  ret2 = foo(1,2);   // int&, 평가되지 않은 표현식(unevaluated expression)
  
  //C++14
  decltype(auto) ret3 = foo(1,2);   // 위와 같은 표현이다.
}
```
