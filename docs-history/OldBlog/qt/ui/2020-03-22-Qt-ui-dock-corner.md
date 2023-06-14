---
title: "dock widget 코너 설정하기"
permalink: qt/ui/dock-corner/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-22 00:00:00 -0000
last_modified_at: 2020-03-22 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
// in QMainWindow
 setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
 setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
 setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
 setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
```

> * [참고사이트](https://doc.qt.io/archives/4.6/qt4-mainwindow.html)