---
title: "qt : Windows에 Shadaw 효과 주기"
permalink: qt/ui/shadow/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-08 00:00:00 -0000
last_modified_at: 2020-04-08 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
#include <dwmapi.h>

// intert shadow
const MARGINS shadow = { 1, 1, 1, 1 };
DwmExtendFrameIntoClientArea(HWND(winId()), &shadow);
```

생성자에 넣으면된다.<br>
설명은 역시 쉽기에 생략!<br>