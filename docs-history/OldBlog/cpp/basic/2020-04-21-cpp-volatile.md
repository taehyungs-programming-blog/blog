---
title: "C++ : volatile"
permalink: cpp/basic/volatile/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-21 00:00:00 -0000
last_modified_at: 2020-04-21 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
---

* [참고사이트](https://dojang.io/mod/page/view.php?id=749)

## 이론

변수를 선언할 때 앞에 volatile을 붙이면 컴파일러는 해당 변수를 최적화에서 제외하여 항상 메모리에 접근하도록 만듭니다.

```cpp
volatile int num1 = 10;
```

## Example

```cpp
int i = 0;

while (i < 10)
    i++;

printf("%d\n", i);    // 10
```

volatile을 쓰지않으면 위의 코드는 컴파일러에 의해서

```cpp
int i = 10;    // 반복문을 없애버리고 10을 할당

printf("%d\n", i);    // 10
```

이렇게 된다.

```cpp
volatile int i = 0;    // volatile로 선언하여 항상 메모리에 접근하도록 만듦

// 항상 i의 메모리에 접근해야 하므로 컴파일러는 반복문을 없애지 않음
while (i < 10)
    i++;

printf("%d\n", i);    // 10
```

volatile은 위와 같은 최적화를 막아준다.