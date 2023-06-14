---
title: "(Qt) 4. QSharedPointer"
permalink: qt/core/QSharedPointer/                # link 직접 지정
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
  - QSharedPointer
category:
  - 정리
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## 사용법

```cpp
QSharedPointer<consumer> c(new consumer, &QObject::deleteLater);
// QSharedPointer<class> name(new class, deleter);
QSharedPointer<consumer> c1(c, &QObject::deleteLater);
c->clear();     // 연결된 pointer가 모두 clear하면 자동 delete
c1->clear();    // 여기서 delete 된다.
```

```cpp
// deleter는 이렇게 구현 가능
void doDeleteLater(test *obj) {
    qInfo() << "Deleting: " << obj;
    obj->deleteLater();
}

QSharedPointer<test> createPointer() {
    test *t = new test(); // the bad way!!!
    t->setObjectName("Test Object");
    QSharedPointer<test> p(t,doDeleteLater); //&QObject::deleteLater
    return p;
}
```

---

## 전체코드

* [Github](https://github.com/8bitscoding/qtci1-4)

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QSharedPointer>
#include "test.h"
#include "consumer.h"

//QSharedPointer will delete the pointer it is holding when it goes out of scope,
//provided no other QSharedPointer objects are referencing it.

// test는 QObject를 상속한다
void doDeleteLater(test *obj) {
    qInfo() << "Deleting: " << obj;
    obj->deleteLater();
}

QSharedPointer<test> createPointer() {
    test *t = new test(); // the bad way!!!
    t->setObjectName("Test Object");
    QSharedPointer<test> p(t,doDeleteLater); //&QObject::deleteLater
    return p;
}

void doStuff(QSharedPointer<consumer> ptr) {
    qInfo() << "In function: " << ptr.data()->tester;

    ptr.clear(); //Does not actually delete!, just removes the reference
}


QSharedPointer<consumer> makeConsumer() {
    QSharedPointer<consumer> c(new consumer, &QObject::deleteLater);
    QSharedPointer<test> t = createPointer();

    c.data()->tester.swap(t);
    doStuff(c);

    return c;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSharedPointer<consumer> consume = makeConsumer();
    qInfo() << "In main: " << consume.data()->tester;

    consume.clear(); //Does not actually delete!, just removes the reference



    return a.exec();
}
```