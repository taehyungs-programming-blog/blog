---
title: "(QML) Pure QML Custom Shapes"
permalink: qt/qml/11/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/11-QMLCanvasDemo)

![](/file/image/qml-cpp-interfacing-11-1.png)

```xml
import QtQuick 2.12

Item {
    Canvas{
        id : root
        width:  300
        height:  300

        onPaint: {
            //Do the drawing
            var context = getContext("2d");
            context.reset();

            var centerX = width/2;
            var centerY = height/2;

            context.beginPath();
            context.fillStyle = "yellowgreen"
            context.moveTo( centerX,centerY);
            context.arc(centerX, centerY, width / 2, 0, Math.PI * 1.5, false);
            context.lineTo(centerX,centerY);
            context.fill();

            context.beginPath();
            context.fillStyle = "red";
            context.moveTo(centerX, centerY);
            context.arc(centerX, centerY, width / 4, Math.PI *1.5, Math.PI, false);
            context.lineTo(centerX, centerY);
            context.fill();
        }

    }

}
```

```xml
import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Circle{

    }
}
```