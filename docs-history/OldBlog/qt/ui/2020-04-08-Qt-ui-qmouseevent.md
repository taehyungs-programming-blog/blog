---
title: "qt : QMouseEvent로 특정지점 클릭 메시지 보내기"
permalink: qt/ui/qmouseevent/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-08 00:00:00 -0000
last_modified_at: 2020-04-08 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
QMouseEvent pressEvent(QEvent::MouseButtonPress, QPointF(100, 100), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
QGuiApplication::sendEvent(this, &pressEvent);
```

쉽기에 설명은 생략!