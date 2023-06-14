---
title: "qt : Widget connect 하기"
permalink: qt/core/widgetconnect/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-21 00:00:00 -0000
last_modified_at: 2020-04-21 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
QCombobox* combo;

// ...

connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(fn(int)));

// ...

void fn(int _m) {}
```