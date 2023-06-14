---
title: "Qt : multi monitor 환경 현재 widget의 monitor 확인"
permalink: qt/ui/multi-monitor/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-25 00:00:00 -0000
last_modified_at: 2020-05-25 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
category:
  - UI
  - windowflags
excerpt: ""
---

```cpp
QDesktopWidget* d = QApplication::desktop(); // screenNumber
int sn = d->screenNumber(this);
int sn2 = d->screenNumber(QWidget);
```