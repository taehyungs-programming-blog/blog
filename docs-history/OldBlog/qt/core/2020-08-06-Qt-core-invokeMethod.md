---
title: "Qt : invokeMethod"
permalink: qt/core/invoke/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-08-06 00:00:00 -0000
last_modified_at: 2020-08-06 00:00:00 -0000
header:
  teaser: /file/image/qt-page-teaser.gif
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
category:
  - invokeMethod
excerpt: ""
---

signal 없이 slot 함수 호출할때 사용

```cpp
MyClass *my;
QMetaObject::invokeMethod( my, "doFunction", Q_ARG( int , param ) );
```

return값

```cpp
QString retVal;
QMetaObject::invokeMethod(obj, "compute", Qt::DirectConnection,
                                  Q_RETURN_ARG(QString, retVal),
                                  Q_ARG(QString, "sqrt"),
                                  Q_ARG(int, 42),
                                  Q_ARG(double, 9.7));
```