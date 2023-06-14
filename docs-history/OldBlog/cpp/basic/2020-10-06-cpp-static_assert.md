---
title: "(C++) static_assert"
permalink: cpp/static_assert/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-06 00:00:00 -0000
last_modified_at: 2020-10-06 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - static_assert
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
// run-time assert
assert(myPointer != NULL;)

// Compile time assert(C++11)
static_assert(sizeof(int) == 4);
```