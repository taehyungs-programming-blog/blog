---
title: "(Qt) 46. QTimer - Timeout"
permalink: qt/core/timeout/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci11-2)

```cpp
#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QTime>

class test : public QObject
{
    Q_OBJECT
public:
    explicit test(QObject *parent = nullptr);

signals:

public slots:
    void timeout();
    void dostuff();

private:
    QTimer timer;
    int number;
};

#endif // TEST_H
```

```cpp
#include "test.h"

test::test(QObject *parent) : QObject(parent)
{
    number = 0;
    timer.setInterval(1000);
    connect(&timer,&QTimer::timeout,this, &test::timeout);
}

void test::timeout()
{
    number++;
    qInfo() << QTime::currentTime().toString(Qt::DateFormat::SystemLocaleLongDate);
    if(number == 5) {
        timer.stop();
        qInfo() << "Complete!";
    }
}

void test::dostuff()
{
    timer.start();
}
```