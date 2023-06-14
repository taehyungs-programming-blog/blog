---
title: "(Qt) memory Patterns"
permalink: qt/desing-patterns/memory-patterns/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-28 00:00:00 -0000
last_modified_at: 2020-12-28 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Desing-Pattern
category:
  - memory Patterns
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

## stack

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/4-1)

C++의 stack 메모리는 C++에 의해 관리되어진다.<br>
하지만 QObject역시 C++의 stack 메모리와 같이 관리되어 질까?

두 가지를 알아야 한다.

1. QObject는 복사될 수 없다.
2. C++에서 stack메모리는 C++에 의해서만 관리(지워질) 수 있다.

```cpp
#include <QCoreApplication>

/*

  What
  The Stack

  Description
  C++ automatic memory managment
  Primarliy deals with scope
  This is basic C++

  Why
  C++ by design, the stack is how applications work

  Example
  In C++ the stack memory is where local variables get stored/constructed.
  The stack is also used to hold parameters passed to functions

  Find stack
  ulimit -s
  ulimit -a

  By design you can not copy or move a QObject
  QObjects are actually rather complex!
  QObjects usually communicate with each other using the signals and slots mechanism
  QObjects are organised in object trees.
  QObjects manages the memory of the children of a QObject

  https://doc.qt.io/qt-5/qobject.html#Q_DISABLE_COPY
  https://www.cleanqt.io/blog/why-qobject-subclasses-are-not-copyable

  Because of Q_DISABLE_COPY we are forced to use pointers or address of!

 */
#include "myclass.h"

/*
MyClass create()
{
    MyClass myclass;
    myclass.setObjectName("my class");
    return myclass; // QObject는 복사될 수 없기에 복사생성자로 리턴이 불가능
}
*/

void testLifetime()
{
    // 일반적 C++의 stack 관리를 보여준다.
    MyClass myclass;
    myclass.setObjectName("my class");

}

void modClass(MyClass *myclass)
{
    myclass->setObjectName("my class");
    // stack 메모리는 코드에 의해서 지워질 수 없다
    //myclass->deleteLater(); // Error : anything on the stack should NOT be deleted!
}

void modRef(MyClass &myclass)
{
    myclass.setObjectName("my class");
    // 위와 같은 문제
    //myclass.deleteLater(); //anything on the stack should NOT be deleted!
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //testLifetime();

    MyClass myclass;
    modClass(&myclass);

    //modRef(myclass);

    return a.exec();
}
```

---

## heap

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/4-2)

heap에서 관리시 메모리를 삭제/카피가 가능해진다.

```cpp
#include <QCoreApplication>

/*

  What
  The heap

  Description
  The "free store" where you can allocate anything you want
  BUT
  You control the memory managment

  Why
  Super fast access vs copying

  Example
  Pointer vs copy
  Copy is SLOW
  Pointers are FAST

 */

#include <QDebug>
#include "myclass.h"

void testCopy(MyClass object)
{
    qInfo() << &object << "len:" << object.data().length();
}

void testPointer(MyClass *object)
{
    //Whats happening here is C++ is giving us an object at the memory location
    object->log("Pointer");
    qInfo() << object << "Length: " << object->data().length();
}

void testAddressOf(MyClass &object)
{
    //Whats happening here is C++ is giving us an object at the memory location
    object.log("AddressOf");
    qInfo() << &object << "Length: " << object.data().length();
}

void testHeap()
{
    MyClass *myclass = new MyClass();
    myclass->log("Hello");

    delete myclass;
}

void testCopyPointer()
{
    MyClass *myclass = new MyClass();
    MyClass *myclass2;

    myclass->log("Hello");

    myclass2 = myclass; //COPY

    delete myclass;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyClass m;
    //Test copy
    //testCopy(m);

    //testPointer(&m); //Basically same things

    //testAddressOf(m);

    //testHeap();

    testCopyPointer();

    return a.exec();
}
```

---

## 메모리 사용의 주의사항

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/4-3)

```cpp
#include <QCoreApplication>

/*

  What
  Memory leaks, stale pointers

  Description
  When we use a pointer, we are required to manage that memory

  Why
  its what makes C++ both powerfull and dangerous

  Examples
  Memory leak

 */

#include <QList>
#include <QDebug>
#include "myclass.h"

void fillList(QList<MyClass*> &list, int max)
{
    for(int i = 0 ; i < max; i++)
    {
        MyClass *myclass = new MyClass(); //We created, we OWN it
        myclass->setObjectName("Object " + QString::number(i));
        list.append(myclass);
    }

}

void demoLeak()
{
    qInfo() << "Making a leak";
    QList<MyClass*> list;
    fillList(list,5);

    list.clear();

    //None of the pointers have been deleted!!!
}

void demoDanglingPointer()
{
    qInfo() << "Making a dangling pointers";
    QList<MyClass*> list;
    fillList(list,5);

    //Delete some memory
    delete list.at(2); //memory is gone!

    qInfo() << list.length();

    foreach(MyClass *item, list)
    {
        qInfo() << item->objectName();
    }

}

void demoCorrect()
{
    qInfo() << "Making a pointers";
    QList<MyClass*> list;
    fillList(list,5);

    qDeleteAll(list); //Deletes the memory
    list.clear(); //Removes the dangling pointers

    qInfo() << list.length();

    foreach(MyClass *item, list)
    {
        qInfo() << item->objectName();
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //demoLeak();
    //demoDanglingPointer();

    demoCorrect();

    return a.exec();
}

```

---

## Parent child 관계

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/4-4)

```cpp
#include <QCoreApplication>

/*

  What
  Parent child memory managment - aka the Qt way

  Description
  Setting a QObjects parent, so when the parent is deleted, all the children are deleted

  Why
  Super easy memory managment

  Example
  Parent and children
QObjects organize themselves in object trees.
When you create a QObject with another object as parent, the object will automatically add itself to the parent's children() list.
The parent takes ownership of the object; i.e., it will automatically delete its children in its destructor.
You can look for an object by name and optionally type using findChild() or findChildren().

 */

#include <QDebug>
#include "myclass.h"

void createChildren(MyClass *parent, int count)
{
    if(!parent) return;
    qInfo() << "Creating children";

    for(int i = 0; i < count; i++)
    {
        MyClass *child = new MyClass(parent); //heap
        child->setObjectName("Child " + QString::number(i));
    }
}

void listChild(MyClass *parent)
{
    if(!parent) return;
    qInfo() << "Listing children";

    foreach(QObject *obj, parent->children())
    {
        MyClass *child = qobject_cast<MyClass*>(obj);
        //test child
        qInfo() << child;
    }
}

void findChild(MyClass* parent, QString name)
{
    if(!parent) return;
    qInfo() << "Find child";

    QList<QObject*> list = parent->findChildren<QObject*>(name);
    qInfo() << "Found " << list.length();
    foreach(QObject *obj, list)
    {
        MyClass *child = qobject_cast<MyClass*>(obj);
        //test child
        qInfo() << "Found: " <<  child;
    }

}

void testQt()
{
    MyClass parent(nullptr); //stack
    parent.setObjectName("parent");
    createChildren(&parent,5);
    listChild(&parent);
    findChild(&parent,"Child 2");
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    testQt();

    return a.exec();
}
```

---

## RAII(Resource acqusition is initialization)

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/4-5)

---

## Scoped pointer

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/4-6)

```cpp
#include <QCoreApplication>

/*

  What
  Scoped Pointers

  Description
  Automatically deleting when it goes out of scope

  Why
  Takes ownership of a pointer and automatically removes it

  Example
  QScopedPointer and QScopedArrayPointer

 */

#include <QDebug>
#include <QScopedPointer>
#include <QScopedArrayPointer>
#include "myclass.h"

MyClass *makeClass(QString name)
{
    //You would be suprised how many applications and libs do this
    return new MyClass(nullptr,name);
}

void testScoped()
{
    //Take ownership as soon as we get it!
    QScopedPointer<MyClass> p(makeClass("my smart pointer"));

    //COOOL
    p->test();

    //goes out of scope and everything is removed
}

void testScopedArray()
{
    //make an array of smart pointers
    int max = 10;
    QScopedArrayPointer<MyClass> p;
    p.reset(new MyClass[max]);

    for(int i = 0; i < max; i++)
    {
        p[i].test();
    }
    //goes out of scope and everything is removed
}

// this struct calls "myCustomDeallocator" to delete the pointer
struct SmartListCleanup
{
    static inline void cleanup(QList<MyClass*> *list)
    {
        qInfo() << "Custom Cleanup";
        qDeleteAll(*list);
        list->clear();
    }
};

void testCustomDelete()
{
    QList<MyClass*> list;
    QScopedPointer<QList<MyClass*>,SmartListCleanup> p(&list);

    for(int i = 0; i < 10; i++)
    {
        list.append(makeClass("List" + QString::number(i)));
    }

    //Do stuff with the list
    foreach(MyClass *item, list)
    {
        item->test();
    }

    //List automatically clears and deletes and pointers
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //testScoped();
    //testScopedArray();
    testCustomDelete();

    return a.exec();
}
```

---

## Shared pointer

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/4-7)

```cpp
#include <QCoreApplication>
/*

  What
  Shared Pointers

  Description
  Pointers that auto-delete when no longer used AND can be used more than once

  Why
  Sometimes multiple things point to the same pointer

  Example
  QSharedPointer
  QSharedPointer will delete the pointer it is holding when it goes out of scope,
  provided no other QSharedPointer objects are referencing it.
  Reference counting
  Also not thread safe as the references might be on different threads

  There is probably a better example out there
  We are focused on QSharedPointer, not so much threads

 */

#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QSharedPointer>
#include "myclass.h"
#include "myrunnable.h"

void test()
{
    qInfo() << "******** Entering test";

    QSharedPointer<MyClass> ptr(new MyClass);
    for(int i = 0; i < QThreadPool::globalInstance()->maxThreadCount(); i++)
    {
        MyRunnable *runnable = new MyRunnable();
        runnable->setAutoDelete(true);
        runnable->setPointer(ptr);
        QThreadPool::globalInstance()->start(runnable);

    }

    qInfo() << "******** Exiting test";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    test();

    return a.exec();
}
```

---

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/4-8)

```cpp
#include <QCoreApplication>

/*
 What
 Smart pointers in the standard library

 Description
 Baked right into C++

 Why
 Just because Qt provides something doesn't mean its the best tool

 Example

 Test cases
 unique_ptr - only one object can point to it
 shared_ptr - multiple objects can point to it
 https://www.geeksforgeeks.org/smart-pointers-cpp/

 */

#include <QDebug>
#include <iostream>
#include <memory>
#include "myclass.h"

using namespace std;

void testUnique()
{
    qInfo() << "Testing Unique";
    unique_ptr<MyClass> p1(new MyClass());
    qInfo() << "p1=" << p1.get();

    p1->test("From pointer 1");

    unique_ptr<MyClass> p2 = move(p1); //take ownership
    qInfo() << "p1=" << p1.get(); //set to nullptr
    qInfo() << "p2=" << p2.get();

    //Test the pointers
    if(p1.get() != nullptr) p1->test("From pointer 1");
    if(p2.get() != nullptr) p2->test("From pointer 2");

}

void testShared()
{
    qInfo() << "Testing shared";
    shared_ptr<MyClass> p1(new MyClass());
    shared_ptr<MyClass> p2 = p1;

    qInfo() << "p1=" << p1.get();
    qInfo() << "p2=" << p2.get();

    //Test the pointers
    if(p1.get() != nullptr) p1->test("From pointer 1");
    if(p2.get() != nullptr) p2->test("From pointer 2");

    qInfo() << "Ref Count" << p1.use_count();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //testUnique();
    testShared();

    return a.exec();
}

```