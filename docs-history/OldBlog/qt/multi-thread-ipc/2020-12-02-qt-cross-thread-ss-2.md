---
title: "(Qt) Cross Thread Signal and Slots - 2"
permalink: qt/mthread-ipc/crossthread-ss-2/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/CrossThreadSignalSlots2)

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "producer.h"
#include "consumer.h"
#include "thread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

private:
    Ui::Widget *ui;
    Producer * producer;
    Consumer * consumer;
    Thread * thread;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QApplication>
#include <QCloseEvent>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << "Ui thread : " << QThread::currentThread();

    producer = new Producer(this);
    ui->verticalLayout->addWidget(producer);

    consumer = new Consumer();

    thread = new Thread(producer);

    connect(thread,&QThread::finished,[=](){
        qDebug() << "Application about to be killed , thread : " << QThread::currentThread();
        QApplication::quit();

    });

    thread->start();

    producer->startProduction();


}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *event)
{
    //Quit thread before killing app
    thread->quit();

    event->accept();
}
```

thread.h

```cpp
#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include "producer.h"

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(Producer * producer,QObject *parent = nullptr);

signals:

public slots:
private:
    Producer * m_producer;

    // QThread interface
protected:
    void run() override;
};

#endif // THREAD_H
```

thread.cpp

```cpp
#include "thread.h"
#include "consumer.h"

Thread::Thread(Producer * producer , QObject *parent) : QThread(parent),
    m_producer(producer)
{

}

void Thread::run()
{
    Consumer consumer;
    connect(m_producer,&Producer::data,&consumer,&Consumer::data);

    //Start our own event loop
    exec();
}
```

producer.h

```cpp
#ifndef PRODUCER_H
#define PRODUCER_H

#include <QPushButton>
#include <QTimer>
#include "customdata.h"

class Producer : public QPushButton
{
    Q_OBJECT
public:
    explicit Producer(QWidget *parent = nullptr);

signals:
    void data(const CustomData &);

public slots:
    void sendData();
    void startProduction();

private:
    int m_integer;
    QTimer * m_timer;
};

#endif // PRODUCER_H
```

producer.cpp

```cpp
#include "producer.h"
#include <QDebug>
#include <QThread>

Producer::Producer(QWidget *parent) : QPushButton(parent)
{
    connect(this,&Producer::clicked,this,&Producer::sendData);
    m_integer = 10;
    setText("Produce");

    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,[=](){

        CustomData cd(22, &m_integer, "Produced data");
        emit data(cd);

    });
}

void Producer::sendData()
{
    qDebug() << "Producer producing data , thread :" << QThread::currentThread();
    CustomData cd(10, &m_integer, "Produced data");
    emit data(cd);
}

void Producer::startProduction()
{
    m_timer->start(1000);
}
```