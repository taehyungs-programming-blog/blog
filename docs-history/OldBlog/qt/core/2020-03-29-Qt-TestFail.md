---
title: "(Qt) 52. Testing failure"
permalink: qt/core/testfail/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-5-3)

```cpp
#include <QCoreApplication>
#include <QTest>
#include "widget.h"

// Use some of the QTEST Macros
// https://doc.qt.io/qt-5/qtest.html
// https://doc.qt.io/qt-5/qtest-tutorial.html

//Update .Pro file:  QT += testlib

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Widget widget;
    widget.setAge(0);
    QTest::qExec(&widget);

    return a.exec();
}
```

```cpp
#include "widget.h"

Widget::Widget(QObject *parent) : QObject(parent)
{
    age = 0;
}

void Widget::setAge(int value)
{
    age = value;
}

void Widget::testFail()
{
    QFAIL("NO REASON JUST FAIL!!!");
}

void Widget::testAge()
{
    if(!age) QFAIL("Age is not set!");
}

void Widget::testWidget()
{

    int value = 45;

    //Make sure the age is valid
    QVERIFY(age > 0 && age < 100);

    //Issue warnings
    if(age > 40) QWARN("Age is over 40!");
    if(age < 21) QFAIL("Must be an adult!");

    //Make sure they are the same
    QCOMPARE(age, value);


}
```