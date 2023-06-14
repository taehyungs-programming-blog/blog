---
title: "(QML) Loading Visual Types with QQuickView"
permalink: qt/qml/16/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-14 00:00:00 -0000
last_modified_at: 2021-02-14 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - 
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/16-QQuickViewLoadQMLDemo)

```cpp
#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QQuickItem>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQuickView view;
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    QObject * rootObject = view.rootObject();
    qDebug() << "Root object name is : " << rootObject->objectName();

    QObject * object = rootObject->findChild<QObject*>("rect");
    if( object){
        QQuickItem * item = qobject_cast<QQuickItem*>(object);

        //Modify its properties
        QColor color(Qt::blue);
        item->setProperty("color",color);
        item->setProperty("width",QVariant::fromValue(600));
        item->setProperty("height",QVariant::fromValue(600));
    }
    return app.exec();
}
```