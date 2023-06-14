---
title: "(Qt) Subclass QThread"
permalink: qt/mthread-ipc/subclass-thread/                # link 직접 지정
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
  - Subclass QThread
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/Subclass-QThread)

Thread를 연결할 Object를 그냥 QThread의 subclass로 만들어 버린다.

---

## 전체코드

Widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <workerthread.h>

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

    void currentCount(int value);

private:
    Ui::Widget *ui;
    WorkerThread * workerThread;
};

#endif // WIDGET_H
```

Widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QThread>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << "Ui thread : " << QThread::currentThread();

    workerThread = new WorkerThread(this);
    connect(workerThread,&WorkerThread::currentCount,this,&Widget::currentCount);

    connect(workerThread,&QThread::started,[](){
        qDebug() << "Thread started";
    });

    connect(workerThread,&QThread::finished,[](){
        qDebug() << "Thread finished";
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{
    workerThread->start();
}

void Widget::on_infoButton_clicked()
{
    qDebug() << "Thread  is running : " << workerThread->isRunning();
}

void  Widget::currentCount(int value){

    ui->progressBar->setValue(value);
    ui->infoLabel->setText(QString::number(value));

}
```

WorkerThread.h

```cpp
#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThread(QObject *parent = nullptr);
    ~WorkerThread() override;

signals:
    void currentCount(int value);

public slots:

    // QThread interface
protected:
    void run() override;
};

#endif // WORKERTHREAD_H
```

WorkerThread.cpp

```cpp
#include "workerthread.h"
#include <QDebug>

WorkerThread::WorkerThread(QObject *parent) : QThread(parent)
{
    qDebug() << "Worker thread constructor running in thread : " << QThread::currentThread();
}

WorkerThread::~WorkerThread()
{
    qDebug() << "Worker thread destructor running in thread : " << QThread::currentThread();
}

void WorkerThread::run()
{
    qDebug() << "Run method of thread running in thread : " << QThread::currentThread();

    for(int i{0} ; i < 1000000001 ; i++){
        /*
             * Only emit signal to send info to ui at 100000 intervals. UI can handle this.
             * Otherwise it is going to freeze.
             * */
        if((i%100000) == 0){
            double percentage = ((i/1000000000.0)) * 100;
            emit currentCount(QVariant::fromValue(percentage).toInt());
        }
    }

    //Start event loop
    //exec() ;
}
```