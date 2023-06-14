---
title: "(Qt) ThreadPool and QRunnable - Async Code"
permalink: qt/mthread-ipc/threadpool-runnable-3/                # link 직접 지정
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
  - Async Code
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/ThreadPool-Runnable-AsyncCode)

```cpp
void Worker::run()
{
    qDebug() << "Worker run method running in thread : " << QThread::currentThread();

    //Because there is no event loop in the threads in here, you can't run async code
        // like QTimer or TCPSocket . If you do, the thread just exits and finishes
    //without doing nothing.

    QTimer * timer = new QTimer();
        QObject::connect(timer,&QTimer::timeout,[](){
            qDebug() << "Time out . Running in thread : " << QThread::currentThread();
        });
        timer->setInterval(1000);
        timer->start();
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

```cpp
#include "worker.h"
#include <QDebug>
#include <QThread>
#include <QApplication>
#include <QMetaObject>
#include <QTimer>
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

    //Because there is no event loop in the threads in here, you can't run async code
        // like QTimer or TCPSocket . If you do, the thread just exits and finishes
    //without doing nothing.

    QTimer * timer = new QTimer();
        QObject::connect(timer,&QTimer::timeout,[](){
            qDebug() << "Time out . Running in thread : " << QThread::currentThread();
        });
        timer->setInterval(1000);
        timer->start();
}
```