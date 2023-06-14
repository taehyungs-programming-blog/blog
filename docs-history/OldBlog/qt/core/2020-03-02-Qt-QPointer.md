---
title: "(Qt) 2. QPointer"
permalink: qt/core/QPointer/                # link 직접 지정
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
  - QPointer
category:
  - 정리
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## 사용법

```cpp
QPointer<QObject> p(obj);
// QPointer<class> name(pointer)
```

동일하게 delete해줘야 함.<br>
뭐... 사실 딱히 써야할 이유는 잘 모르겠음.<br>

딱히 사용이유를 찾으라면 비교연산자가 모두 구현되어 있기에 포인터 비교가 된다는 점정도??

> * [참고사이트](https://doc.qt.io/qt-5/qpointer.html)

---

## 전체코드

```cpp
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QObject *obj = new QObject(nullptr);
    obj->setObjectName("My Object");

    QPointer<QObject> p(obj);

    test t;
    t.widget = p;
    t.useWidget();

    if(p.data()) qInfo() << p.data();

    delete  obj;
    t.useWidget();

    return a.exec();
}
```

```cpp
#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QDebug>
#include <QPointer>

class test : public QObject
{
    Q_OBJECT
public:
    explicit test(QObject *parent = nullptr);

    QPointer<QObject> widget;

    void useWidget();

signals:

public slots:
};

#endif // TEST_H
```

```cpp
#include "test.h"

test::test(QObject *parent) : QObject(parent)
{

}

void test::useWidget()
{
    //see if there is a pointer!

    if(!widget.data()) {

        qInfo() << "No Pointer!!!";
        return;
    }

    qInfo() << "Widget = " << widget.data();

    //Access the pointer
    widget.data()->setObjectName("used Widget!");
}
```

> * [Github](https://github.com/8bitscoding/qtci1-2)