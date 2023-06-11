---
layout: default
title: "24. constexpr"
parent: (C++)
grand_parent: (C++)
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* `constexpr`은 **컴파일 시간 상수**만 넣을 수 있다.

```cpp
int n = 10;

const int c1 = 10;    // 컴파일 시간에 알 수 있다.(컴파일 시간 상수)
const int c2 = n;     // 컴파일 시간에 알 수 없다. (실행시간 상수)

constexpr int c3 = 10;    // Ok
constexpr int c4 = n;     // Compile Error, 컴파일 시간 상수만 넣을 수 있다.
```

## constexpr은 보통 이렇게 사용된다.

* `constexpr` 상수
* `constexpr function`
* `if constexpr`

```cpp
constexpr int add(int a, int b)
{
  return a + b;
}

int main()
{
  int x = 1;
  int y = 1;
  
  int n1 = add(1, 1);   // 컴파일 시 인자의 값을 알 수 있다면 컴파일 시 계산
  int n2 = add(x, y);   // 그렇지 않다면 런타임에 계산
}
```

```cpp
#include <iostream>

constexpr int factorial(int n)
{
  // file open, new 등을 할 순 없다(컴파일 타임에 결정이 되지 못함.)
  if(n == 1)
    return 1;
  return n * factorial(n-1);
}

int main()
{
  int n = factorial(5);
  
  std::cout << n << std::endl;
}
```

```cpp
if constexpr(n == 0)
// 컴파일 시간에 조건식을 조사하라.
// 단, 컴파일 시간에 조건검사가 가능하게 변수가 지정이 되어 있어야함.
```