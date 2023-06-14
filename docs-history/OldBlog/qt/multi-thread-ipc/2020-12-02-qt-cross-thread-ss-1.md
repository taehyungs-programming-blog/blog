---
title: "(Qt) Cross Thread Signal and Slots - 1"
permalink: qt/mthread-ipc/crossthread-ss-1/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/CrossThreadSignalSlots1)

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "producer.h"
#include "consumer.h"
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

private:
    Ui::Widget *ui;
    Producer * producer;
    Consumer * consumer;
    QThread * thread;

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

    thread = new QThread(this);

    connect(thread,&QThread::finished,[=](){
        qDebug() << "Application about to be killed , thread : " << QThread::currentThread();
        QApplication::quit();

    });


    //Do queued connection
    connect(producer,&Producer::data,consumer,&Consumer::data);

    consumer->moveToThread(thread);

    thread->start();
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

producer.h

```cpp
#ifndef PRODUCER_H
#define PRODUCER_H

#include <QPushButton>
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

private:
    int m_integer;
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
}

void Producer::sendData()
{
    qDebug() << "Producer producing data , thread :" << QThread::currentThread();
    CustomData cd(10, &m_integer, "Produced data");
    emit data(cd);
}
```

consumer.h

```cpp
#ifndef CONSUMER_H
#define CONSUMER_H

#include <QObject>
#include <customdata.h>

class Consumer : public QObject
{
    Q_OBJECT
public:
    explicit Consumer(QObject *parent = nullptr);

signals:

public slots:
    void data(const CustomData &cd);
private:
    int m_counter;
};

#endif // CONSUMER_H
```

consumer.cpp

```cpp
#include <QThread>
#include <QDebug>
#include "consumer.h"

Consumer::Consumer(QObject *parent) : QObject(parent)
{
    m_counter = 0;
}

void Consumer::data(const CustomData &cd)
{
    qDebug() << "Consumer message : " << cd.m_integer << *cd.m_pointer << cd.m_string
                 << " thread : " << QThread::currentThread();

    //Kill the thread
    if( ++m_counter > 10){
        qDebug() << "Consumer , threa about to be killed";
        thread()->quit();
    }
}
```