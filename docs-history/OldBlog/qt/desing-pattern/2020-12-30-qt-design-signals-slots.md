---
title: "(Qt) Signals and Slots"
permalink: qt/desing-patterns/signals-slots/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-30 00:00:00 -0000
last_modified_at: 2020-12-30 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Desing-Pattern
category:
  - Signals and Slots
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

## signals and slots basics

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/5-1)

기본적 signals and slots 사용법

```cpp
#include <QCoreApplication>

/*

  What
  Signals and slots

  Description
  The moc that connects objects

  Why
  Inter-object communication

  Exmaple
  Simple example of signals and slots
  look at MOC

  /home/rootshell/Code/Qt/build-qt5dp-5-1-Desktop_Qt_5_12_3_GCC_64bit-Debug
  Look at the MOC files
  look at the CPP files for each

*/

#include <QObject>
#include <QDebug>
#include "producer.h"
#include "consumer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Producer producer;
    Consumer consumer;

    //Connect the objects
    QObject::connect(&producer,&Producer::readyProduct, &consumer,&Consumer::readyProduct);
    QObject::connect(&a,&QCoreApplication::aboutToQuit, &producer,&Producer::stop);

    producer.start();

    return a.exec();
}
```

```cpp
#include "producer.h"

Producer::Producer(QObject *parent) : QObject(parent)
{
    m_timer.setInterval(3000);

    //Connect the two objects
    connect(&m_timer,&QTimer::timeout,this,&Producer::produced);
}

void Producer::start()
{
    qInfo() << this << "Producer starting";
    m_timer.start();
}

void Producer::stop()
{
    m_timer.stop();
    qInfo() << this << "Producer stopped";
}

void Producer::produced()
{
    qInfo() << this << "Producing item";
    QSharedPointer<MyClass> ptr(new MyClass());
    emit readyProduct(ptr);
    qInfo() << this << "Item produced";
}
```

---

## Disconnecting

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/5-2)

```cpp
#include "alarm.h"

Alarm::Alarm(QObject *parent) : QObject(parent)
{
    qInfo() << this << "Constructed";
}

Alarm::~Alarm()
{
    qInfo() << this << "Destroyed";
}

QTime Alarm::limit() const
{
    return m_limit;
}

void Alarm::setLimit(const QTime &limit)
{
    m_limit = limit;
    qInfo() << this << "Alarm set for" << m_limit;
}

void Alarm::timeout()
{
    //qInfo() << this << "Timeout";
    QTimer *timer = qobject_cast<QTimer*>(sender());
    if(!timer) return;

    if(QTime::currentTime() >= m_limit)
    {
        //Sound the alarm
        qInfo() << this << "Alarm";

        disconnect(timer,&QTimer::timeout,this,&Alarm::timeout);
    }
}
```

---

## Auto connecting

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/5-3)

```cpp
#ifndef AUTOCONNECT_H
#define AUTOCONNECT_H

/*
 Connects the signals and slots of two objects
 signal test();
 slot test();
*/
#include <QObject>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QChildEvent>
#include <QMetaObject>
#include <QMetaMethod>

class AutoConnect
{
public:
    static void connect(QObject *sender, QObject *receiver)
    {
        if(!sender) return;
        if(!receiver) return;

        QList<QByteArray> sendrSignals = scanType(sender,QMetaMethod::Signal);
        QList<QByteArray> receiverSlots = scanType(receiver,QMetaMethod::Slot);

        foreach(QByteArray signature, sendrSignals)
        {
            if(receiverSlots.contains(signature))
            {
                qInfo() << "Auto connecting:" << signature;
                //SIGNAL AND SLOT MACROS:
                //#define SLOT(a) "1"#a
                //#define SIGNAL(a) "2"#a
                //QObject::connect(sender,SIGNAL(readyRead()),receiver,SLOT(readyRead()));
                //qInfo() << "Matched: " << signature;
                //qInfo() << "SIGNAL: " << SIGNAL(readyRead());
                //qInfo() << "SLOT: " << SLOT(readyRead());

                //QObject::connect(sender,&QObject::signal,receiver,&Qobject::slot);
                //QObject::connect(sender,SIGNAL(test()),receiver, SLOT(test()));
                QObject::connect(sender,"2"+signature, receiver,"1"+signature);
            }
        }
    }

private:
    static QList<QByteArray> scanType(QObject *object, QMetaMethod::MethodType type)
    {
        QList<QByteArray> list;
        if(!object) return list;

        const QMetaObject *mo = object->metaObject();
        if(!mo) return list;
        for(int i = mo->methodOffset(); i < mo->methodCount(); i++)
        {
            QMetaMethod method = mo->method(i);
            if(method.methodType() == type)
            {
                list.append(method.methodSignature());
            }
        }

        return list;
    }

};

#endif // AUTOCONNECT_H
```

---

## Sender function

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/5-4)

---

## Connection types

connect 연결시 thread어떻게 사용할지 결정

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/5-5)

```cpp
#include <QCoreApplication>

/*

  What
  Signal & Slot Connection Types

  Description
  Connections between signals and slots can have differnt connection types

  Why
    We are about to find out!

  Example

  https://doc.qt.io/qt-5/signalsandslots.html
  https://doc.qt.io/qt-5/qt.html#ConnectionType-enum

 */

#include <QDebug>
#include <QThread>
#include "myclass.h"

void test(MyClass *source, MyClass *destination, Qt::ConnectionType type = Qt::ConnectionType::AutoConnection, QThread *thread = nullptr)
{
    QObject::connect(source,&MyClass::test, destination,&MyClass::onTest, type);
    if(thread)
    {
        qInfo() << &destination << "moving to:" << thread;
        destination->moveToThread(thread);
    }
    source->perform();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyClass source(nullptr,"source");
    MyClass destination(nullptr,"destination");

    QThread::currentThread()->setObjectName("Main Thread");
    QThread thread;
    thread.start();
    thread.setObjectName("Seperate Thread");

    //---Non-Threaded---

    //Auto
    //test(&source,&destination);

    //DirectConnection
    //test(&source,&destination,Qt::DirectConnection);

    //QueuedConnection
    //test(&source,&destination,Qt::QueuedConnection);

    //BlockingQueuedConnection - will deadlock
    //test(&source,&destination,Qt::BlockingQueuedConnection);

    //UniqueConnection
    //test(&source,&destination,Qt::UniqueConnection);

    //---Threaded---

    //Auto threaded - works across thread - sometimes
    //test(&source,&destination,Qt::AutoConnection, &thread);

    //DirectConnection threaded - signal triggers slot but sender is 0x0
    //test(&source,&destination,Qt::DirectConnection, &thread);

    //QueuedConnection threaded -  works as expected (perfered for cross thread)
    //test(&source,&destination,Qt::QueuedConnection, &thread);

    //BlockingQueuedConnection threaded - a bit dangerous, can lock the signaling thread
    //test(&source,&destination,Qt::BlockingQueuedConnection, &thread);

    //UniqueConnection threaded - works as expected
    test(&source,&destination,Qt::UniqueConnection,&thread);



    return a.exec();
}
```

---

## signals and slots inside lib

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/5-6)

```cpp
#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDebug>

#include "../mylib/mylib_global.h"
#include "../mylib/mylib.h"

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    void start();
    void stop();

signals:
    void limit();

public slots:
    void started();
    void stopped();
    void triggered(QString value);

private:
    Mylib m_mylib;
    int m_count;
};

#endif // CLIENT_H
```

```cpp
#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    //Signals and slots work as expected
    connect(&m_mylib,&Mylib::started,this,&Client::started);
    connect(&m_mylib,&Mylib::stopped,this,&Client::stopped);
    connect(&m_mylib,&Mylib::triggered,this,&Client::triggered);
    connect(this,&Client::limit,&m_mylib,&Mylib::stop);
    m_count = 0;
}

void Client::start()
{
    m_count = 0;
    m_mylib.start();
}

void Client::stop()
{
    m_mylib.stop();
}

void Client::started()
{
    qInfo() << "Started";
}

void Client::stopped()
{
    qInfo() << "Stopped";
}

void Client::triggered(QString value)
{
    m_count++;
    if(m_count >= 5)
    {
        emit limit();
        return;
    }
    qInfo() << m_count << value;
}
```