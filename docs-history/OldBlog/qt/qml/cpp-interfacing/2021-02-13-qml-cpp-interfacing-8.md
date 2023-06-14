---
title: "(QML) Custom Painted Types : Images-Demo"
permalink: qt/qml/8/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/8-PaintOnImageDemo)

![](/file/image/qml-cpp-interfacing-8-1.png)

```xml
import QtQuick 2.12
import QtQuick.Window 2.12
import com.blikoon.logos 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Logo{
        id : mLogo1
        text: "Learn QT"
        bgColor: "blue"
        textColor: "red"
    }

    Logo{
        id : mLogo2
        anchors.top : mLogo1.bottom
        anchors.topMargin: 20
        text: "Go Fluid"
        bgColor: "beige"
        textColor: "black"
        topic: Logo.QTQUICK
    }
}
```

```cpp
#ifndef LEARNQTLOGO_H
#define LEARNQTLOGO_H

#include <QObject>
#include <QImage>
#include <QQuickPaintedItem>

class LearnQtLogo : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor NOTIFY bgColorChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QtTopic topic READ topic WRITE setTopic NOTIFY topicChanged)
public:
    explicit LearnQtLogo(QQuickItem *parent = nullptr);

    void paint(QPainter *painter);

    enum QtTopic{
        QTCPP,
        QTQUICK
    };
    Q_ENUM(QtTopic)

    QColor bgColor() const;

    QColor textColor() const;

    QString text() const;

    QtTopic topic() const;

    void setBgColor(QColor bgColor);

    void setTextColor(QColor textColor);

    void setText(QString text);

    void setTopic(QtTopic topic);

signals:

    void bgColorChanged(QColor bgColor);

    void textColorChanged(QColor textColor);

    void textChanged(QString text);

    void topicChanged(QtTopic topic);

private :
    QColor m_bgColor;
    QColor m_textColor;
    QString m_text;
    QtTopic m_topic;
};

#endif // LEARNQTLOGO_H
```

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "learnqtlogo.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<LearnQtLogo>("com.blikoon.logos",1,0,"Logo");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```