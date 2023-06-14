---
title: "(Qt) 51. Basic tests"
permalink: qt/core/basictest/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-5-2)

역시 코드만 봐서는 잘 모르겠음...

```
17:51:46: Starting C:\GIT\build-qtca-5-2-Desktop_Qt_5_13_2_MSVC2017_64bit-Debug\debug\qtca-5-2.exe ...
Constructor
********* Start testing of Dog *********
Config: Using QtTest library 5.13.2, Qt 5.13.2 (x86_64-little_endian-llp64 shared (dynamic) debug build; by MSVC 2017)
QINFO  : Dog::initTestCase() initTestCase
PASS   : Dog::initTestCase()
QINFO  : Dog::bark() init
QINFO  : Dog::bark() bark bark bark
QINFO  : Dog::bark() cleanup
PASS   : Dog::bark()
QINFO  : Dog::rollover() init
QINFO  : Dog::rollover() *rolls*
QINFO  : Dog::rollover() cleanup
PASS   : Dog::rollover()
QINFO  : Dog::cleanupTestCase() cleanupTestCase
PASS   : Dog::cleanupTestCase()
Totals: 4 passed, 0 failed, 0 skipped, 0 blacklisted, 2ms
********* Finished testing of Dog *********
```

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QTest>
#include "dog.h"

//Update .Pro file:  QT += testlib

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Dog dog;
    QTest::qExec(&dog);

    return a.exec();
}
```

```cpp
#ifndef DOG_H
#define DOG_H

#include <QObject>
#include <QDebug>

class Dog : public QObject
{
    Q_OBJECT
public:
    explicit Dog(QObject *parent = nullptr);

signals:

public slots:

private slots:
    //Called before the first test
    void initTestCase();

    //Called before each test
    void init();

    //Called after each test
    void cleanup();

    //Called after the last test
    void cleanupTestCase();

    //Our custom slot to be tested
    void bark();

    //Another custom slot to test
    void rollover();


};

#endif // DOG_H
```

```cpp
#include "dog.h"

Dog::Dog(QObject *parent) : QObject(parent)
{
    qInfo() << "Constructor";
}

void Dog::initTestCase()
{
    qInfo() << "initTestCase";
}

void Dog::init()
{
    qInfo() << "init";
}

void Dog::cleanup()
{
    qInfo() << "cleanup";
}

void Dog::cleanupTestCase()
{
    qInfo() << "cleanupTestCase";
}

void Dog::bark()
{
    qInfo() << "bark bark bark";
}

void Dog::rollover()
{
    qInfo() << "*rolls*";
}
```