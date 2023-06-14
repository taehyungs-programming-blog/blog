---
title: "Qt : qt dynamic properties and stylesheets"
permalink: qt/ui/dynamic-p-and-s/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-11-14 00:00:00 -0000
last_modified_at: 2020-11-14 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
  - UI
category:
  - properties
  - stylesheets
excerpt: ""
classes: wide
---

* [참고사이트](https://wiki.qt.io/Dynamic_Properties_and_Stylesheets)

---

There are limitations to using this trick. The main one is that the style will not update itself automatically when the value of a property referenced from the style sheet changes. Instead, you must manually trigger an update of the visual appearance of a widget when a styled property changes. For example:

요약하자면, 코드에서 stylesheet를 수정하기 위해서는 `unpolish` 후 `polish` 를 해줘라

```cpp
myLineEdit->setProperty("urgent", true); 
myLineEdit->style()->unpolish(myLineEdit); 
myLineEdit->style()->polish(myLineEdit);
```

Note that this must be done in the widget to which the style was applied. QStyle::polish accepts either a QWidget or a QApplication as a parameter.