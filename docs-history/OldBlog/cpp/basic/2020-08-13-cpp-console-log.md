---
title: "C++ : Console log 출력"
permalink: cpp/mfc/console-log/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-08-13 00:00:00 -0000
last_modified_at: 2020-08-13 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - console log
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://memoryfilm.tistory.com/2)

```cpp
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
```