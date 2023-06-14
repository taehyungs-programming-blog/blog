---
title: "(Qt) 36. qDeleteAll"
permalink: qt/core/qdeleteall/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci9-1)

---

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QtAlgorithms>
#include "test.h"

// qDeleteAll

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QList<test*> list;
    for(int i = 0; i< 10; i++) {
        test *t = new test(); //notice no parent!!!
        t->setObjectName(QString::number(i));
        list.append(t);
    }

    qInfo() << list.at(0);

    //Delete them all!
    qDeleteAll(list);

    //Will crash!
    //qInfo() << list.at(0);

    //Objects are still in the list but invalid pointers - clear the list!
    list.clear();

    return a.exec();
}
```