---
title: "(QML) Custom Painted Types : Draw a Bar chart"
permalink: qt/qml/9/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-13 00:00:00 -0000
last_modified_at: 2021-02-13 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - Paint
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/9-BarChartDemo)

![](/file/image/qml-cpp-interfacing-9-1.png)

```xml
import QtQuick 2.12
import QtQuick.Window 2.12
import com.blikoon.charts 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    StairChart{

    }
}
```

```cpp
#ifndef STAIRCHART_H
#define STAIRCHART_H

#include <QObject>
#include <QQuickPaintedItem>

class StairChart : public QQuickPaintedItem
{
    Q_OBJECT


public:
    explicit StairChart(QQuickItem *parent = nullptr);

    void paint(QPainter * painter);

signals:

public slots:

private :
    void drawStair(QPainter *painter, QRect mRect, QColor mColor, qreal barValue,int startingPoint);
    void drawScale(QPainter *painter, QRect mRect);
};

#endif // STAIRCHART_H
```