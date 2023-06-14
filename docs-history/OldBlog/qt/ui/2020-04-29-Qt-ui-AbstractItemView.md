---
title: "Qt - UI : QSS와 QAstractItemView"
permalink: qt/ui/subclassing/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-29 00:00:00 -0000
last_modified_at: 2020-04-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
comboBox_mic->setItemDelegate(new QStyledItemDelegate());

선언시 그리기를 추상화 가능
```

```css
/* in qss */

QComboBox QAbstractItemView::item
{
    height: 46px;
    background: #282844;
    border-top: 1px solid #323247;
    padding-left: 14px;
    padding-right: 14px;
}

QComboBox QAbstractItemView::item:selected 
{
    background: #373751;
    color: rgb(255,255,255);
}
```