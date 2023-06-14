---
title: "(Qt) QThread asynchronous(Move To Thread)"
permalink: qt/mthread-ipc/a-move-to-thread/                # link 직접 지정
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
  - asynchronous
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/AsyncMoveToThread)

역시 비동기 방식으로 처리방법

---

## 전체코드

Widget.h

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

public slots:
    void doWork();

};

#endif // WORKER_H
```

Widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include "worker.h"
#include <QDebug>
#include <QThread>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << "Ui thread : " << QThread::currentThread();

    thread = new QThread(this);


}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{
    Worker * worker = new Worker;

    worker->moveToThread(thread);

    connect(thread,&QThread::started,worker,&Worker::doWork);
    connect(thread,&QThread::finished,[](){
        qDebug() << "Thread finished";
    });
    connect(thread,&QThread::finished,worker,&Worker::deleteLater);

    thread->start();

}

void Widget::on_stopButton_clicked()
{
    thread->exit();
}

void Widget::on_infoButton_clicked()
{
    qDebug() << "Thread running : " << thread->isRunning();
}
```