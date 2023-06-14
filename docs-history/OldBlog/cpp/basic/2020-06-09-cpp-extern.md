---
title: "C++ : extern"
permalink: cpp/basic/extern/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-09 00:00:00 -0000
last_modified_at: 2020-06-09 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - extern
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://dojang.io/mod/page/view.php?id=802)

```cpp
// B.cpp
#include <stdio.h>

int num1 = 10;      // 여기서 선언

void printNumber()
{
    print("%d\n", num1);
}
```

```cpp
// A.cpp
#include <stdio.h>

extern int num1;        // 여기서 사용

int main()
{
    print("%d\n", num1);
    return 0;
}
```