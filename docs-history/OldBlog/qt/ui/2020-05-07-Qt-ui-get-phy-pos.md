---
title: "Qt : physical position 구하기"
permalink: qt/ui/get-physical-pos/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-07 00:00:00 -0000
last_modified_at: 2020-05-07 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
---

* [참고사이트](https://wikidocs.net/36590)

![](/file/image/physical-pos.png)

```cpp
QPoint p = ui->centralwidget->mapToGlobal( ui->centralwidget->pos() );
```