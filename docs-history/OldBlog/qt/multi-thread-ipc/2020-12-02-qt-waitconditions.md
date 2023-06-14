---
title: "(Qt) WaitConditions"
permalink: qt/mthread-ipc/waitconditions/                # link 직접 지정
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
  - WaitConditions
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/WaitConditions)

Read하는 쪽이 여러개이고 Write는 하나로 고정하는 Example

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QWaitCondition>
#include <QMutex>
#include "producer.h"
#include "consumer.h"

namespace Ui {
class Widget;
}

const int M_BUFFER_SIZE = 6000;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_startButton_clicked();

private:
    Ui::Widget *ui;

    int totalData;
    char buffer[M_BUFFER_SIZE];
    QWaitCondition bufferFull;
    QWaitCondition bufferEmpty;
    QMutex mutex;
    int usedSlots;
    Producer * producer;
    Consumer * consumer;
};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    totalData = 100000;
    usedSlots = 0;


    producer = new Producer(buffer,M_BUFFER_SIZE,&usedSlots,totalData,
                                 &mutex,&bufferFull,&bufferEmpty);

    consumer = new Consumer(buffer,M_BUFFER_SIZE,&usedSlots,totalData,
                                 &mutex,&bufferFull,&bufferEmpty);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{
    producer->start();
    consumer->start();

    producer->wait();
    consumer->wait();
}
```

consumer.h

```cpp
#ifndef CONSUMER_H
#define CONSUMER_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class Consumer : public QThread
{
    Q_OBJECT
public:
    explicit Consumer(char * buffer, int bufferSize,
                      int * usedSlots, int totalData,
                      QMutex * mutex,
                      QWaitCondition * bufferFull,QWaitCondition * bufferEmpty,
                      QObject *parent = nullptr);

signals:

public slots:

    // QThread interface
protected:
    void run() override;

private:
    char * m_buffer;
    int m_buffer_size;
    int * m_used_slots;
    int m_total_data;
    QMutex * m_mutex;
    QWaitCondition * m_buffer_full;
    QWaitCondition * m_buffer_empty;

};

#endif // CONSUMER_H
```

consumer.cpp

```cpp
#include "consumer.h"
#include <QDebug>

Consumer::Consumer(char * buffer, int bufferSize,
                   int * usedSlots, int totalData,
                   QMutex * mutex,
                   QWaitCondition * bufferFull,QWaitCondition * bufferEmpty,
                   QObject *parent) : QThread(parent),
    m_buffer(buffer), m_buffer_size(bufferSize),
    m_used_slots(usedSlots),m_total_data(totalData),
    m_mutex(mutex),
    m_buffer_full(bufferFull),
    m_buffer_empty(bufferEmpty)
{

}

void Consumer::run(){

    for (int i = 0; i < m_total_data; ++i) {
        m_mutex->lock();

        if((*m_used_slots) == 0){
            m_buffer_empty->wait(m_mutex);//FREEZE CODE EXECUTION IN THE THEAD
        }

        m_mutex->unlock();

        //Write data to console
        qDebug() << "Consumer data : " << m_buffer[i % m_buffer_size];


        m_mutex->lock();
        --(*m_used_slots);
        m_buffer_full->wakeAll();
        m_mutex->unlock();
    }
}
```

producer.h

```cpp
#ifndef PRODUCER_H
#define PRODUCER_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class Producer : public QThread
{
    Q_OBJECT
public:
    explicit Producer(char * buffer, int bufferSize,
                      int * usedSlots, int totalData,
                      QMutex * mutex,
                      QWaitCondition * bufferFull,QWaitCondition * bufferEmpty,
                      QObject *parent = nullptr);

signals:

public slots:

    // QThread interface
protected:
    void run() override;

private:
    char * m_buffer;
    int m_buffer_size;
    int * m_used_slots;
    int m_total_data;
    QMutex * m_mutex;
    QWaitCondition * m_buffer_full;
    QWaitCondition * m_buffer_empty;
};

#endif // PRODUCER_H
```

producer.cpp

```cpp
#include "producer.h"
#include <QRandomGenerator>

Producer::Producer(char * buffer, int bufferSize,
                   int * usedSlots, int totalData,
                   QMutex * mutex,
                   QWaitCondition * bufferFull,QWaitCondition * bufferEmpty,
                   QObject *parent) : QThread(parent),
    m_buffer(buffer), m_buffer_size(bufferSize),
        m_used_slots(usedSlots),m_total_data(totalData),
        m_mutex(mutex),
        m_buffer_full(bufferFull),
        m_buffer_empty(bufferEmpty)
{

}

void Producer::run() {


     for (int i = 0; i < m_total_data; ++i) {
         m_mutex->lock();

         if((*m_used_slots) == m_buffer_size){
             m_buffer_full->wait(m_mutex);
         }
         m_mutex->unlock();

         //Write data to the buffer
         m_buffer[i % m_buffer_size] = "BACK"[QRandomGenerator::global()->bounded(4)];

         m_mutex->lock();
         ++(*m_used_slots);
         m_buffer_empty->wakeAll();
         m_mutex->unlock();
     }
}
```