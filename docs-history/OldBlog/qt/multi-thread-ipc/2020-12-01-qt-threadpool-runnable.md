---
title: "(Qt) hreadPool and QRunnuable"
permalink: qt/mthread-ipc/threadpool-runnable/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/ThreadPool-Runnable)

## Threadpool

* A number of threads is reserved in your Qt application, to run long running operation for you
* All you need to do is wrap your long operation in a QRunnable subclass
* And throw your runnable task in a pool of threads to run it

대략 돌고있는 Thread확인후 비어있으면 그걸쓰겠다.

---

## 전체코드

Worker.h

```cpp
#ifndef WORKER_H
#define WORKER_H

#include <QRunnable>

class Worker : public QRunnable     // QRunnable 상속
{

public:
    explicit Worker();
    ~Worker() override;


    // QRunnable interface
public:
    void run() override;
};

#endif // WORKER_H
```

Worker.cpp

```cpp
#include "worker.h"
#include <QDebug>
#include <QThread>

Worker::Worker()
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
            qDebug() << "Current percentage : " << percentage;
        }
    }
}
```

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
    ~Widget();

private slots:
    void on_startButton_clicked();

    void on_infoButton_clicked();

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
```

Widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include "worker.h"
#include <QThreadPool>
#include <QDebug>

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

void Widget::on_startButton_clicked()
{
    Worker * worker = new Worker();

    QThreadPool::globalInstance()->start(worker);
}

void Widget::on_infoButton_clicked()
{

}
```