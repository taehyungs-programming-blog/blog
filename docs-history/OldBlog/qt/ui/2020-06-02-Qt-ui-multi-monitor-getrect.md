---
title: "Qt : multi monitor 환경에서 특정 모니터 rect 받기"
permalink: qt/ui/multi-monitor-getrect/                # link 직접 지정
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
  - UI
  - multi monitor
  - getRect
excerpt: ""
---

```cpp
QDesktopWidget* d = QApplication::desktop(); // screenNumber
int sn = d->screenNumber(this);
int sn2 = d->screenNumber(QObject);

QRect screenRect = QApplication::desktop()->screenGeometry(sn);
```

* [참고사이트](https://stackoverflow.com/questions/48742152/force-qt-window-to-specific-screen)