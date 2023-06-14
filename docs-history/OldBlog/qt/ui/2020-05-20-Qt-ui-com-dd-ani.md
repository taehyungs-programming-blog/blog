---
title: "Qt : QCombobox drop down animation 삭제"
permalink: qt/ui/com-dd-animation/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-20 00:00:00 -0000
last_modified_at: 2020-05-20 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
category:
  - UI
  - qcombobox
  - qapplication
excerpt: ""
---

```cpp
QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);
```