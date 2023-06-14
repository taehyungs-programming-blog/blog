---
title: "(Qt) Thread Patterns"
permalink: qt/desing-patterns/thread-patterns/                # link 직접 지정
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
  - Thread Patterns
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

## QThread 재 상속

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/6-1)

```cpp
#include <QCoreApplication>

/*

  What
  Inheriting QThread

  Description
  Not the best idea but it can be done

  Why
  Java style threads
  Note - This is not the best way!!!
  You can easily break the thread

  Example
  Inherit QThread
   https://doc.qt.io/qt-5/qthread.html

 */

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QSharedPointer>
#include "myclass.h"

MyClass *thread;
QTimer *timer;

void timeout()
{
    if(thread) return;
    if(timer) return;

    if(thread->isRunning() == true && thread->isFinished() == false)
    {
        qInfo() << "Thread is running!";
    }

    if(thread->isRunning() == false && thread->isFinished() == true)
    {
        qInfo() << "Thread is finished!";
        timer->stop();
        thread->deleteLater();
        timer->deleteLater();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    thread = new MyClass(&a);
    timer = new QTimer(&a);
    thread->setObjectName("Child Thread");

    timer->setInterval(1000);
    QObject::connect(timer, &QTimer::timeout, timeout);
    timer->start();
    thread->start(); //QThread::start calls QThread::run()

    //Do other things in the background


    return a.exec();
}
```

```cpp
#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QDebug>
#include <QThread>

class MyClass : public QThread
{
    Q_OBJECT
public:
    explicit MyClass(QObject *parent = nullptr);
    ~MyClass();
signals:

    // QThread interface
protected:
    void run() override;
};

#endif // MYCLASS_H
```

```cpp
#include "myclass.h"

MyClass::MyClass(QObject *parent) : QThread(parent)
{
    qInfo() << this << "Created on" << QThread::currentThread();
}

MyClass::~MyClass()
{
    qInfo() << this << "Destoyed on" << QThread::currentThread();
}

void MyClass::run()
{
    qInfo() << this << "Run on" << QThread::currentThread();

    //Do thread stuff here
    QThread::currentThread()->msleep(5000);

    qInfo() << this << "Finished on" << QThread::currentThread();
}
```

---

## moveToThread 사용기

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/6-2)

- QThread에서 사용되는 Thread를 지정가능

```cpp
#include <QCoreApplication>

/*

    What
    QObject moveto QThread

    Description
    Not the best idea but it can be done

    Why
    You do not want to risk breaking the functionality of QThread

    Example
    move an object to a thread

*/

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include "myclass.h"

QThread *thread;
QTimer *timer;
MyClass *myclass;

void timeout()
{
    if(!thread) return;
    if(!timer) return;

    if(thread->isRunning() == true && thread->isFinished() == false)
    {
        qInfo() << "Thread is Running...";
    }

    if(thread->isRunning() == false && thread->isFinished() == true)
    {
        qInfo() << "Thread is Finished!";
        timer->stop();
        thread->deleteLater();
        timer->deleteLater();
        myclass->deleteLater();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread::currentThread()->setObjectName("Main thread");
    thread = new QThread(&a);
    thread->setObjectName("Child thread");

    timer = new QTimer(&a);
    myclass = new MyClass(nullptr); // NO PARENT!!!!

    QObject::connect(timer, &QTimer::timeout, timeout); //Auto connection
    QObject::connect(thread, &QThread::started, myclass,&MyClass::run, Qt::QueuedConnection); //cross thread signal / slot
    QObject::connect(myclass, &MyClass::finished, thread,&QThread::quit, Qt::QueuedConnection); //cross thread signal / slot

    myclass->moveToThread(thread); //Will fail if there is a parent!
    timer->setInterval(1000);

    timer->start();
    thread->start(); //QThread::start calls QThread::run()

    //Do other things in the background


    return a.exec();
}
```

```cpp
#include "myclass.h"

MyClass::MyClass(QObject *parent) : QObject(parent)
{
    qInfo() << this << "Created on" << QThread::currentThread();
}

MyClass::~MyClass()
{
    qInfo() << this << "Destoyed on" << QThread::currentThread();
}

void MyClass::run()
{
    qInfo() << this << "Run on" << QThread::currentThread();

    //Do thread stuff here
    QThread::currentThread()->msleep(5000);

    qInfo() << this << "Finished on" << QThread::currentThread();

    //use signals and slots
    emit finished();
}
```

![](/file/image/qt-design-thread-1.png)

---

## QThreadPool

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/6-3)

`QRunnable`로 상속된 Thread를 `QThreadPool`에서 효율적으로 thread관리를 해준다.

```cpp
#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QThread>
#include <QRunnable>
#include <QDebug>
#include <QRandomGenerator>

class MyClass : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit MyClass(QObject *parent = nullptr);
    ~MyClass();

signals:
    void started();
    void finished();
    void progress(int value);


    // QRunnable interface
public:
    void run() override;
};

#endif // MYCLASS_H
```

```cpp
void MyClassManager::run()
{
    qInfo() << "Max Threads" << QThreadPool::globalInstance()->maxThreadCount();
    for(int  i = 0; i < 25; i++)
    {
        MyClass *myclass = new MyClass(nullptr); //no parent - on main thread
        myclass->setAutoDelete(false); //true will cause issues in this video

        myclass->setObjectName("MyClass" + QString::number(i));

        connect(myclass,&MyClass::started,this,&MyClassManager::started, Qt::QueuedConnection);
        connect(myclass,&MyClass::progress,this,&MyClassManager::progress, Qt::QueuedConnection);
        connect(myclass,&MyClass::finished,this,&MyClassManager::finished, Qt::QueuedConnection);

        QThreadPool::globalInstance()->start(myclass); //the apps global thread pool
    }
}
```

---

## Qt Concurrent

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/6-4)

- 간단하게 thread를 생성(정확히는 concurrent)

```cpp
#include <QCoreApplication>

/*

  What
  Qt Concurrent

  Description
  High level threading

  Why
  Threads are complicated

  Example
  Folder Counter
  QT += concurrent

 */

#include <QDebug>
#include <QFileInfo>
#include <QTextStream>
#include <QThread>
#include "filescanner.h"

QString getPath()
{
    QTextStream stream(stdin);

    qInfo() << "Please enter a path:";
    QString value = stream.readLine();
    QFileInfo fi(value);
    if(!fi.exists())
    {
        qWarning() << "Not a valid path!";
        value.clear();
    }

    return value;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread::currentThread()->setObjectName("Main Thread");

      QString path = getPath();
      while (path.isEmpty()) {
          path = getPath();
      }

      qInfo() << path;

      FileScanner fs;
      fs.scan(path);

    return a.exec();
}
```

```cpp
#ifndef FILESCANNER_H
#define FILESCANNER_H

#include <QObject>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QtConcurrent>
#include <QFuture>

class FileScanner : public QObject
{
    Q_OBJECT
public:
    explicit FileScanner(QObject *parent = nullptr);

    void scan(QString path);

signals:
    void updated(int count);

private:
    int performscan(QString path);
    qint64 m_count;

};

#endif // FILESCANNER_H
```

```cpp
#include "filescanner.h"

FileScanner::FileScanner(QObject *parent) : QObject(parent)
{

}

void FileScanner::scan(QString path)
{
    qInfo() << "scan on" << QThread::currentThread();
    m_count = 0;

    //This could take a long time
    QFuture<int> future = QtConcurrent::run(this,&FileScanner::performscan, path);

    //Main thread is not blocked, we can do other things
    qInfo() << "Main thread is free to do other things...";


    qInfo() << "Result:" << future.result(); //get the result from the Concurrent thread
}

int FileScanner::performscan(QString path)
{
    // Runs on the threadPool

    int value = 0;
    QDir dir(path);
    if(!dir.exists()) return -1;
    //qInfo() << "performing scan on" << QThread::currentThread();

    QFileInfoList list = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
    value = list.length();

    emit updated(value);

    foreach(QFileInfo item, list)
    {
        value = value + performscan(item.filePath());
    }

    return value;
}
```

---

## thread를 통해 ui 업데이트

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/6-5)

- main thread가 block 되지 않고 ui 업데이트 방법

---

## qml으로 thread ui 업데이트

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/6-6)