---
title: "(Qt) 3. QScopedPointer"
permalink: qt/core/QScopedPointer/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-10-01 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
tag:
  - qt
  - QScopedPointer
category:
  - 정리
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## 사용법

```cpp
QScopedPointer<test> mypointer(new test());
// QScopedPointer<class> name(new class)
```

delete해 주지 않아도 된다.

---

## 전체코드

* [Github](https://github.com/8bitscoding/qtci1-3)

```cpp
// main.cpp
#include <QCoreApplication>
#include <QDebug>
#include <QScopedPointer>
#include "test.h"

// The QScopedPointer class stores a pointer to a dynamically allocated object,
// and deletes it upon destruction!!!

void mytest(QScopedPointer<test> &mypointer) {
    qInfo() << mypointer.data();
}

void useit(test* obj) {
    if(!obj) return;

    qInfo() << "Using" << obj;
}

void doStuff() {
    //test *t = new test(); // dangling pointer!!!
    QScopedPointer<test> mypointer(new test());

    useit(mypointer.data());
    mytest(mypointer);

    //pointer is automatically deleted!!!
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    for(int i = 0; i < 100; i++) {
        doStuff();
    }

    doStuff();

    return a.exec();
}
```

```cpp
// test.h
#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QDebug>

class test : public QObject
{
    Q_OBJECT
public:
    explicit test(QObject *parent = nullptr);
    ~test();
signals:

public slots:
};

#endif // TEST_H
```

```cpp
// test.cpp
#include "test.h"

test::test(QObject *parent) : QObject(parent)
{
    qInfo() << "Constructed" << this;
}

test::~test()
{
    qInfo() << "Deconstructed" << this;
}
```