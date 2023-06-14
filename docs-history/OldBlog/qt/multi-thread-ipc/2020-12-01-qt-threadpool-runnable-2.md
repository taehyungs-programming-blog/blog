---
title: "(Qt) ThreadPool and QRunnable - Sending feedback to ui"
permalink: qt/mthread-ipc/threadpool-runnable-2/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-01 00:00:00 -0000
last_modified_at: 2020-12-01 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Multi Thread
category:
  - thread-pool
  - runnable
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/ThreadPool-Runnable-FeedBack)

Event 보내는 방법

```cpp
void Worker::run()
{
    qDebug() << "Worker run method running in thread : " << QThread::currentThread();

    for(int i{0} ; i < 1000000001 ; i++){

        if((i%100000) == 0){

            double percentage = ((i/1000000000.0)) * 100;
           // qDebug() << "Current percentage : " << percentage;

            /*
            QApplication::postEvent(m_receiver,
                         new ProgessEvent(QVariant::fromValue(percentage).toInt()));

            */

            // Widget의 gotUpdate함수를 호출
            QMetaObject::invokeMethod(m_receiver,"gotUpdate",
                                      Qt::QueuedConnection,
                                      Q_ARG(int,QVariant::fromValue(percentage).toInt()));
        }
    }
}
```

```cpp
void Widget::gotUpdate(int value)
{
    ui->progressBar->setValue(value);
}
```

---

```cpp
// 이벤트를 보냄
QApplication::postEvent(m_receiver,
                new ProgessEvent(QVariant::fromValue(percentage).toInt()));
```

```cpp
bool Widget::event(QEvent *event)
{
    if(event->type() == static_cast<QEvent::Type>(ProgessEvent::EventId)){

        ProgessEvent * progressEvent = static_cast<ProgessEvent *>(event);

        //qDebug() << "Widget , progress is : " << progressEvent->progress();

        ui->progressBar->setValue(progressEvent->progress());

        return  true;
    }

    return QWidget::event(event);
}
```

---

## 전체코드

Widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

public slots :
    void gotUpdate(int value);

private slots:
    void on_startButton_clicked();
    void on_infoButton_clicked();

private:
    Ui::Widget *ui;

    // QObject interface
public:
    bool event(QEvent *event) override;
};

#endif // WIDGET_H
```

Widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include "worker.h"
#include "progressevent.h"
#include <QThreadPool>
#include <QDebug>
#include <QEvent>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug() << "Ui thread :" << QThread::currentThread();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::gotUpdate(int value)
{
    ui->progressBar->setValue(value);
}

void Widget::on_startButton_clicked()
{
    Worker * worker = new Worker(this);

    QThreadPool::globalInstance()->start(worker);
}

void Widget::on_infoButton_clicked()
{
    qDebug() << "ThreadPool max thread count : " << QThreadPool::globalInstance()->maxThreadCount();

    qDebug()  << " Ideal thread count : " << QThread::idealThreadCount();
}

bool Widget::event(QEvent *event)
{
    if(event->type() == static_cast<QEvent::Type>(ProgessEvent::EventId)){

        ProgessEvent * progressEvent = static_cast<ProgessEvent *>(event);

        //qDebug() << "Widget , progress is : " << progressEvent->progress();

        ui->progressBar->setValue(progressEvent->progress());

        return  true;
    }

    return QWidget::event(event);
}
```

Progressevent.h

```cpp
#ifndef PROGESSEVENT_H
#define PROGESSEVENT_H

#include <QEvent>

class ProgessEvent : public QEvent
{
public:
    enum {EventId = QEvent::User +1};
    ProgessEvent( int progress = 0);

    int progress() const;

private:
    int m_progress;
};

#endif // PROGESSEVENT_H
```

Progressevent.cpp

```cpp
#include "progressevent.h"

ProgessEvent::ProgessEvent(int progress) : QEvent(static_cast<Type>(EventId)),
    m_progress(progress)
{

}

int ProgessEvent::progress() const
{
    return m_progress;
}
```

Worker.h

```cpp
#ifndef WORKER_H
#define WORKER_H

#include <QRunnable>
#include <QObject>

class Worker : public QRunnable
{

public:
    explicit Worker(QObject * receiver);
    ~Worker() override;


    // QRunnable interface
public:
    void run() override;

private:
    QObject * m_receiver;
};

#endif // WORKER_H
```

Worker.cpp

```cpp
#include "worker.h"
#include <QDebug>
#include <QThread>
#include <QApplication>
#include <QMetaObject>
#include <QVariant>
#include "progressevent.h"

Worker::Worker(QObject * receiver) : m_receiver(receiver)
{
    qDebug() << "Worker constructor running in thread : " << QThread::currentThread();
}

Worker::~Worker(){
    qDebug() << "Worker destructor running in thread : " << QThread::currentThread();

}

void Worker::run()
{
    qDebug() << "Worker run method running in thread : " << QThread::currentThread();

    for(int i{0} ; i < 1000000001 ; i++){

        if((i%100000) == 0){

            double percentage = ((i/1000000000.0)) * 100;
           // qDebug() << "Current percentage : " << percentage;

            /*
            QApplication::postEvent(m_receiver,
                         new ProgessEvent(QVariant::fromValue(percentage).toInt()));

            */
            QMetaObject::invokeMethod(m_receiver,"gotUpdate",
                                      Qt::QueuedConnection,
                                      Q_ARG(int,QVariant::fromValue(percentage).toInt()));
        }
    }
}
```