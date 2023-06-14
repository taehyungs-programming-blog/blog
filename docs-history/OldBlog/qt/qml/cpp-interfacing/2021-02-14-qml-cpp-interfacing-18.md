---
title: "(QML) Invoking QML Methods"
permalink: qt/qml/18/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/18-InvokeQMLMethodDemo)

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject * rootObject = engine.rootObjects()[0];

    QObject * funcContext = rootObject->findChild<QObject*>("deep2");

    if( funcContext){
        qDebug() << "Found the object";
        QVariant returnValue;
        QVariant parameter = "C++ Parameter";

        QMetaObject::invokeMethod(funcContext,"qmlFunction",
                                  Q_RETURN_ARG(QVariant,returnValue),
                                  Q_ARG(QVariant,parameter)
                                  );
        qDebug() << "This is C++, return value is : " << returnValue.toString();

    }
    return app.exec();
}
```