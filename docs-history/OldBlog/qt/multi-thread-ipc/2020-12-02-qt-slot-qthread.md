---
title: "(Qt) Slot In QThread Subclass"
permalink: qt/mthread-ipc/slot-qthread/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-02 00:00:00 -0000
last_modified_at: 2020-12-02 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Multi Thread
category:
  - Cross Thread
  - Signal and Slots
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/SlotInQThreadSubclass)

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "thread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    Thread * thread;
};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << "GUI thread : " << QThread::currentThread();

    thread = new Thread(this);
    thread->start();
}

Widget::~Widget()
{
    delete ui;
}
```

thread.h

```cpp
#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QTimer>

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QObject *parent = nullptr);

signals:

public slots:
    void increment();

    // QThread interface
protected:
    void run() override;

private:
    QTimer * m_timer;
    int m_count;
};

#endif // THREAD_H
```

thread.cpp

```cpp
#include "thread.h"
#include <QDebug>

Thread::Thread(QObject *parent) : QThread(parent),
    m_count(0)
{
    qDebug()  << "Thread constructor on thread : " << QThread::currentThread();
}

void Thread::increment()
{
    qDebug() << "Increment called on thread : " << QThread::currentThread();
    m_count++;

    if(m_count == 5){
        m_timer->stop();
        thread()->quit();
    }
}

void Thread::run() {
    //Managed thread

    qDebug() << "Run called on thread : " << QThread::currentThread();
    m_timer = new QTimer();
    connect(m_timer,&QTimer::timeout,this,&Thread::increment);
    m_timer->start(1000);
    exec();
}
```