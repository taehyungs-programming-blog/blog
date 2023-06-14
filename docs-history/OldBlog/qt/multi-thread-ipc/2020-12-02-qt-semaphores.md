---
title: "(Qt) Semaphores"
permalink: qt/mthread-ipc/semaphores/                # link 직접 지정
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
  - semaphores
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/Semaphores)

Read하는 쪽이 여러개이고 Write는 하나로 고정하는 Example

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSemaphore>
#include <QVector>
#include "producer.h"
#include "consumer.h"

const int BUFFER_SIZE = 6;

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

private:
    Ui::Widget *ui;
    bool atEnd;
    QSemaphore * freeSpace;
    QSemaphore * avSpace;
    QVector<int> dataSource;
    int  bufferArray [BUFFER_SIZE];
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

    dataSource << 1 << 2 << 34 <<55 << 4<< 11 << 22 << 34 <<55 <<
                      4<< 1 << 2 << 34 <<55 << 4<< 1 << 2 << 34 <<55 << 4<<2222;

    atEnd = false;


    freeSpace = new QSemaphore(BUFFER_SIZE);

    avSpace = new QSemaphore(0);

    producer = new Producer(dataSource,bufferArray,BUFFER_SIZE,freeSpace,avSpace,&atEnd);
    consumer = new Consumer(bufferArray,BUFFER_SIZE,freeSpace,avSpace,&atEnd);
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

    atEnd = false;
}
```

producer.h

```cpp
#ifndef PRODUCER_H
#define PRODUCER_H

#include <QThread>
#include <QSemaphore>
#include <QVector>

class Producer : public QThread
{
    Q_OBJECT
public:
    explicit Producer(const QVector<int> & dataSource,
                      int * buffer,int bufferSize,QSemaphore * freeSpace,
                     QSemaphore * availableSpace,
                     bool * atEnd,QObject *parent = nullptr);

signals:

public slots:

    // QThread interface
protected:
    void run() override;

private:
    QVector<int> m_data_source;
    int * m_buffer;
    int m_BUFFER_SIZE;
    QSemaphore * m_free_space;// Space where the producer can write data
    QSemaphore * m_available_space;//Space where consumer can read data from
    bool * m_at_end;
};

#endif // PRODUCER_H
```

producer.cpp

```cpp
#include "producer.h"

Producer::Producer(const QVector<int> & dataSource,  int * buffer,
                   int bufferSize,QSemaphore *freeSpace,
                   QSemaphore *availableSpace, bool *atEnd,QObject *parent) :
    QThread(parent),
    m_data_source(dataSource),
    m_buffer(buffer),
    m_BUFFER_SIZE(bufferSize),
    m_free_space(freeSpace),
    m_available_space(availableSpace),
    m_at_end(atEnd)
{

}

void Producer::run()
{
    for(int i{0} ; i < m_data_source.length() ; i++){

        m_free_space->acquire();
        //WRITE DATA

        m_buffer[ i % m_BUFFER_SIZE] = m_data_source.at(i);

        if(i == m_data_source.length() -1)
            *m_at_end = true;

        m_available_space->release();
    }
}
```

consumer.h

```cpp
#ifndef CONSUMER_H
#define CONSUMER_H

#include <QThread>
#include <QSemaphore>

class Consumer : public QThread
{
    Q_OBJECT
public:
    explicit Consumer(int *buffer, int bufferSize,QSemaphore * freeSpace,
                      QSemaphore * availableSpace,
                      bool * atEnd,QObject *parent = nullptr);

signals:

public slots:

    // QThread interface
protected:
    void run() override;

private:
    int * m_buffer;
    int m_BUFFER_SIZE;
    QSemaphore * m_free_space;// Space where the producer can write data
    QSemaphore * m_available_space;//Space where consumer can read data from
    bool * m_at_end;
};

#endif // CONSUMER_H
```

consumer.cpp

```cpp
#include "consumer.h"
#include <QDebug>

Consumer::Consumer(int *buffer, int bufferSize,QSemaphore * freeSpace,
                   QSemaphore * availableSpace,
                   bool * atEnd,QObject *parent) :
    QThread(parent),
    m_buffer(buffer),m_BUFFER_SIZE(bufferSize),
    m_free_space(freeSpace),
    m_available_space(availableSpace),
    m_at_end(atEnd)
{

}

void Consumer::run()
{

    int i{0};

       while(!(*m_at_end) || m_available_space->available()){
           m_available_space->acquire();
           //READ DATA

           qDebug() << "[" << i << "]" << m_buffer[i];//Consume data

           i =((i+1) % (m_BUFFER_SIZE));

           m_free_space->release();

       }
}
```