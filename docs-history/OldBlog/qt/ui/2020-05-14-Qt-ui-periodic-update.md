---
title: "Qt : 주기적 UI update 호출 방법"
permalink: qt/ui/periodie-update/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-14 00:00:00 -0000
last_modified_at: 2020-05-14 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
category:
  - UI
  - update
  - QCoreApplication
excerpt: ""
---

```cpp
update();
QCoreApplication::processEvents();
// paintEvent처리를 기다려주는 역할을 한다.
```