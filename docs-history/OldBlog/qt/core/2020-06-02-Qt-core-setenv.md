---
title: "Qt : multi monitor 환경에서 특정 모니터 rect 받기"
permalink: qt/core/setenv/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-02 00:00:00 -0000
last_modified_at: 2020-06-02 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
category:
  - Core
  - set Env
excerpt: ""
---

```cpp
qputenv("QT_AUTO_SCREEN_SCALE_FACTOR",  "1");
```

* [참고사이트](https://doc.qt.io/qt-5/highdpi.html)