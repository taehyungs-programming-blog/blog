---
title: "(C++) NULL vs nullptr"
permalink: cpp/NULL-nullptr/                # link 직접 지정
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
  - NULL
  - nullptr
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
void foo(int i) { cout << "foo_int" << endl; }
void foo(char* pc) { cout << "foo_char*" << endl; }

int main() {
    foo(NULL);          // Ambiguity
    foo(nullptr);       // call foo(char*)
}
```