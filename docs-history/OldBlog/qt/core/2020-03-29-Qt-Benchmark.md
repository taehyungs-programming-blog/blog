---
title: "(Qt) 54. Benchmarking"
permalink: qt/core/benchmarking/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-5-5)

```
18:01:13: Starting C:\GIT\build-qtca-5-5-Desktop_Qt_5_13_2_MSVC2017_64bit-Debug\debug\qtca-5-5.exe ...
********* Start testing of Widget *********
Config: Using QtTest library 5.13.2, Qt 5.13.2 (x86_64-little_endian-llp64 shared (dynamic) debug build; by MSVC 2017)
PASS   : Widget::initTestCase()
PASS   : Widget::testFor()
RESULT : Widget::testFor():
     0.00044 msecs per iteration (total: 58, iterations: 131072)
PASS   : Widget::testForEach()
RESULT : Widget::testForEach():
     0.000785 msecs per iteration (total: 103, iterations: 131072)
PASS   : Widget::testString()
RESULT : Widget::testString():
     0.00021 msecs per iteration (total: 57, iterations: 262144)
FAIL!  : Widget::testComp() Compared values are not the same
   Actual   (him): "Bryan"
   Expected (her): "Tammy"
..\qtca-5-5\widget.cpp(56) : failure location
PASS   : Widget::cleanupTestCase()
Totals: 5 passed, 1 failed, 0 skipped, 0 blacklisted, 895ms
********* Finished testing of Widget *********
18:01:14: C:/GIT/build-qtca-5-5-Desktop_Qt_5_13_2_MSVC2017_64bit-Debug/debug/qtca-5-5.exe exited with code 1
```

```cpp
  
#include <QtTest>
#include "widget.h"

//Update .Pro file:  QT += testlib

QTEST_MAIN(Widget);
```

```cpp
#include "widget.h"

Widget::Widget(QObject *parent) : QObject(parent)
{

}

void Widget::testFor()
{
    QVector<int> list;
    list.fill(0,100);

    //Called multiple times!
    QBENCHMARK
    {
        for (int i = 0; i < list.size();i++)
        {
            //Do Stuff
        }
    }
}

void Widget::testForEach()
{
    QVector<int> list;
    list.fill(0,100);

    //Called multiple times!
    QBENCHMARK
    {
        foreach(int value, list)
        {
            //Do Stuff
        }
    }
}

void Widget::testString()
{
    QString him = "Bryan";
    QString her = "Tammy";

    QBENCHMARK
    {
        int ret = him.compare(her);
    }
}

void Widget::testComp()
{
    QString him = "Bryan";
    QString her = "Tammy";

    QBENCHMARK
    {
        QCOMPARE(him, her);
    }
}
```