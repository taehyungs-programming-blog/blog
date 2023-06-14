---
title: "(Qt) Move To Thread"
permalink: qt/mthread-ipc/move-to-thread/                # link 직접 지정
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
  - Move To Thread
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/MoveToThread)

Object의 Thread를 별도로 지정하고 Signal and Slot을 연결할수 있음을 보여준다.

---

## 전체코드

Widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_startButton_clicked();

    void on_infoButton_clicked();

    void countDone();

    void currentCount(int value);

    void threadFinished();

private:
    Ui::Widget *ui;
    QThread * workerThread;
};

#endif // WIDGET_H
```

Widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include "worker.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << "Widget Constructor running in GUI thread : " << thread();
    qDebug() << "Current thread in Widget constructor : " << QThread::currentThread();

    workerThread = new QThread(this);
    connect(workerThread,&QThread::finished,this,&Widget::threadFinished);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{

    Worker * worker = new Worker;//Main thread

    worker->moveToThread(workerThread);

    connect(workerThread,&QThread::started,worker,&Worker::doCounting);
    connect(worker,&Worker::currentCount,this,&Widget::currentCount);
    connect(worker,&Worker::countDone,this,&Widget::countDone);
    connect(workerThread,&QThread::finished,worker,&Worker::deleteLater);


    workerThread->start();

}

void Widget::on_infoButton_clicked()
{
    qDebug() << " Thread running : " << workerThread->isRunning();
}

void Widget::countDone()
{
    qDebug() << "Widget, count Done";
    workerThread->exit();
}

void Widget::currentCount(int value)
{
    ui->progressBar->setValue(value);
    ui->infoLabel->setText(QString::number(value));

}

void Widget::threadFinished()
{
    qDebug() << "Thread finished. Thread : " << thread();
    qDebug() << "Thread finished.Current Thread : " << QThread::currentThread();
}
```

Worker.h

```cpp
#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

signals:
    void currentCount(int value);
    void countDone();
public slots:
    void doCounting();
};

#endif // WORKER_H

```

Worker.cpp

```cpp
#include "worker.h"
#include <QVariant>
#include <QThread>
#include <QDebug>

Worker::Worker(QObject *parent) : QObject(parent)
{
    qDebug() << "Worker constructor running in thread : " << QThread::currentThread();
}

Worker::~Worker()
{
    qDebug() << "Worker destroyed, destructor running in thread : " << QThread::currentThread();
}

void Worker::doCounting()
{

    qDebug() << "Worker counting method running in thread : " << QThread::currentThread();
    //Code here is going to run in secondary/background thread

    for(int i{0} ; i < 1000000001 ; i++){

        double percentage = (i/ 1000000000.0) * 100.0;

        //qDebug() << "Percentage : " << percentage;

        if((i% 100000) ==0){
            emit currentCount(QVariant::fromValue(percentage).toInt());
        }
    }
    emit countDone();
}
```