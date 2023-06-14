---
title: "(Qt) 53. Data driven tests"
permalink: qt/core/datadriven/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-5-4)

```cpp
#include <QCoreApplication>
#include <QTest>
#include "widget.h"

//Update .Pro file:  QT += testlib

//Replaces the main
QTEST_MAIN(Widget);
```

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QObject>
#include <QDebug>
#include <QTest>

class Widget : public QObject
{
    Q_OBJECT
public:
    explicit Widget(QObject *parent = nullptr);

signals:

public slots:

private slots:
    void testAge_data();
    void testAge();


};

#endif // WIDGET_H
```

```cpp
#include "widget.h"

Widget::Widget(QObject *parent) : QObject(parent)
{

}

void Widget::testAge_data()
{
    qInfo() << "Generating data...";

    QTest::addColumn<QString>("name");
    QTest::addColumn<int>("age");

    QTest::addRow("Invalid") << "Bob" << 190;
    QTest::addRow("Old") << "Bryan" << 44;
    QTest::addRow("Young") << "Heather" << 25;
    QTest::addRow("Under age") << "Rango" << 14;
    QTest::addRow("Retired") << "Grandma" << 90;

    qInfo() << "Data generated!";
}

void Widget::testAge()
{
    //Get the row data
    QFETCH(QString,name);
    QFETCH(int,age);

    qInfo() << "Testing age " << name << " is " << age;

    if(age < 1 || age > 100) QFAIL("Invalid Age!");
    if(age < 21) QFAIL("Must be an adult!");
    if(age > 40) QWARN("Getting Old!");
    if(age > 65) qInfo() << "This person is retired";

}

```