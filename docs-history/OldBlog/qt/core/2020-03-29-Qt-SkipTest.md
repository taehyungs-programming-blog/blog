---
title: "(Qt) 55. Skipping tests and expecting failure"
permalink: qt/core/skiptest/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-5-6)

```cpp
#include <QTest>
#include "widget.h"

//Update .Pro file:  QT += testlib

QTEST_MAIN(Widget);
```

```cpp
#include "widget.h"

Widget::Widget(QObject *parent) : QObject(parent)
{

}

void Widget::test()
{
    QFETCH(int, value);

    //Skip 5
    if(value == 5) QSKIP("Skipping 5");

    //Exits
    qInfo() << "Testing: " << value;
}

void Widget::test_data()
{
    qInfo() << "Generating data...";
    QTest::addColumn<int>("value");

    for (int i = 0; i < 10; i++)
    {
        QString name = QString::number(i);
        QByteArray ba = name.toLatin1();
        const char *c_str = ba.data();
        QTest::newRow(c_str) << i;
    }
}

void Widget::testFail()
{
    int current = 6;
    int supported = 6;
    QCOMPARE(current,supported);

    //Test previous
    QEXPECT_FAIL("","Version 5 is not supported",Continue);
    QCOMPARE(current,5); //Will Fail
}
© 2020 GitHub, Inc.
```