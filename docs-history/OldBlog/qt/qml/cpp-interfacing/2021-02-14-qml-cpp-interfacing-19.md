---
title: "(QML) Handling a QML Signal in C++"
permalink: qt/qml/19/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/19-QMLSignalCppSlotDemo)

```cpp
#ifndef APPWRAPPER_H
#define APPWRAPPER_H

#include <QObject>
#include <QQmlApplicationEngine>

class AppWrapper : public QObject
{
    Q_OBJECT
public:
    explicit AppWrapper(QObject *parent = nullptr);

    bool initialize();

signals:

public slots:
private slots :
    void respondToClick(QString msg, const QVariant & object);

private:
    QQmlApplicationEngine mEngine;
};

#endif // APPWRAPPER_H

```

```cpp
#include <QDebug>
#include "appwrapper.h"

AppWrapper::AppWrapper(QObject *parent) : QObject(parent)
{

}

bool AppWrapper::initialize()
{
    mEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (mEngine.rootObjects().isEmpty())
        return false;

    QObject * rootObject = mEngine.rootObjects()[0];
    connect(rootObject,SIGNAL(qmlSignal(QString,QVariant)),
            this,SLOT(respondToClick(QString,QVariant)));

    return true;
}

void AppWrapper::respondToClick(QString msg, const QVariant &object)
{
    qDebug() << "The message is : " << msg;

    QObject * mObject = object.value<QObject*>();

    qDebug() << "The property is : " << mObject->property("mProp").toString();


}

```