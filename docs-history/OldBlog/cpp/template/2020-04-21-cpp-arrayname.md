---
title: "C++ Template : 배열의 이름은 주소일까?"
permalink: cpp/template/arrayname/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-21 00:00:00 -0000
last_modified_at: 2020-06-17 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
category:
  - template
  - type deduction
excerpt: ""
---

```cpp
int main()
{
  int x[3] = {1,2,3};
  int *p = x;   // 배열의 이름은 무조건 배열의 주소인가? -> 여기서 부터 시작한다
}
```

```cpp
int main()
{
  int n ;
  int *p1 = &n;
  
  double d;
  double* p2 = &d;
  
  int x[3] = {1,2,3}; 
  // x의 타입 : int[3]
  int *p3[3] = &x;   // 이렇게 주소를 담을 수 있을까?
  // 되긴한데... 컴파일러가 헷갈리니깐
  int (*p3)[3] = &x;  // 요렿게 표현하자
}
```

```cpp
int main()
{
  int n ;
  int n2 = n;
  
  double d;
  double d2 = d;
  
  int x[3] = {1,2,3}; 
  int x2[3] = x1;       // 배열은 메모리를 많이 먹기에 이런식의 초기화를 지원하지 않음.
  
  int *p1 = x1;    // 단, 배열의 초기값을 암시적 형변환하여 전달 가능
```

아니 뭔 소리야 똑같은 말아님??

배열의 주소나 배열의 첫 번째 주소나??

```cpp
#include <stdio.h>

int main()
{
  int x[3] = {1,2,3};
  
  int (*p1)[3] = &x;    // 배열의 주소
  int *p2 = x;          // 배열의 첫 번째 주소
  
  printf("%p\n", p1);
  printf("%p\n", p2);
  // 똑같은디??
  
  p1++;   // 12 bytes plus
  p2++;   // 4 bytes plus
  // 이 차이이다
  
  (*p1)[0] = 10;    // p1은 배열이기에 역참조해서 특정 요소에 접근해야함.
  *p2 = 10;         // p2는 배열의 첫 번째 요소의 주소이기에 접근후 바로 값 변경가능
}
```