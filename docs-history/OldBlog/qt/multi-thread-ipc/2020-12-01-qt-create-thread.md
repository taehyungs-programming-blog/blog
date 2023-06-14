---
title: "(Qt) Create Thread"
permalink: qt/mthread-ipc/create-thread/                # link 직접 지정
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
  - Create Thread
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/CreateThread)

---

## 왜 Qt Thread를 써야하나?

* Blend well with other parts or Qt (Signals & slot)
* Lots of Qt code already using Qt Threads
* Team coding standards
* It is possible to use Qt Threads and std::thread at the same time

---

```cpp
// 전역으로 선언된 함수 counting1에도 thread연결 가능
thread = QThread::create(counting1, 10000);

// 진역함수 counting호출 방법
thread = QThread::create([=](){
    counting();
});

// connect로 thread의 함수 signaling 가능
connect(thread,&QThread::started,[](){
    qDebug() << "Thread started";
});

connect(thread,&QThread::finished,[](){
    qDebug() << "Thread finished";
});

// 메모리 관리를 위해 finished가 호출ㄹ되면 deleteLater와 connect
connect(thread, &QThread::finished, thread, &QThread::deleteLater);

thread->start();
```

```cpp
//2.Named lambda function

auto countlambda = [](int count){
    for(int i{0} ; i < count ; i ++){
        qDebug() << "countlambda counting...";
        qDebug() << "Counting : " << i <<
                    " thread :" << QThread::currentThread() << " id : " <<
                    QThread::currentThreadId();
    }
};

thread = QThread::create(countlambda,1000000);
```

```cpp
//3. Non named lambda function

thread = QThread::create([](){
    for(int i{0} ; i < 100000 ; i ++){
        qDebug() << "Counting : " << i <<
                    " thread :" << QThread::currentThread() << " id : " <<
                    QThread::currentThreadId();
    }

});
```

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

    void counting();

private slots:
    void on_startButton_clicked();

private:
    Ui::Widget *ui;
    QThread * thread;
};

#endif // WIDGET_H
```

Widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

void counting1(int count){
    for(int i{0} ; i < count ; i ++){
        qDebug() << "Counting : " << i << "thread : " <<  QThread::currentThread();
    }
}



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug() << "Main thread " << QThread::currentThread();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::counting()
{
    for(int i{0} ; i < 10000 ; i ++){
            qDebug() << "Counting method called : " << i <<
                        " thread :" << QThread::currentThread() << " id : " <<
                        QThread::currentThreadId() <<
                        "Thread is running : " << thread->isRunning();
        }
}

void Widget::on_startButton_clicked()
{


    /*
    //0.Freeze the UI
    for(int i{0} ; i < 1000000 ; i ++){
        qDebug() << " counting in ui thread...";
        qDebug() << "Counting : " << i <<
                    " thread :" << QThread::currentThread() << " id : " <<
                    QThread::currentThreadId();
    }

    */


    /*
    //1 .Global function
    thread = QThread::create(counting1,10000);
    */



    /*
    //2.Named lambda function

    auto countlambda = [](int count){
        for(int i{0} ; i < count ; i ++){
            qDebug() << "countlambda counting...";
            qDebug() << "Counting : " << i <<
                        " thread :" << QThread::currentThread() << " id : " <<
                        QThread::currentThreadId();
        }
    };

    thread = QThread::create(countlambda,1000000);
    */

    //3. Non named lambda function

    thread = QThread::create([](){
        for(int i{0} ; i < 100000 ; i ++){
            qDebug() << "Counting : " << i <<
                        " thread :" << QThread::currentThread() << " id : " <<
                        QThread::currentThreadId();
        }

    });


    //4.Member function, call from lamba function
    thread = QThread::create([=](){
        counting();
    });


    connect(thread,&QThread::started,[](){
        qDebug() << "Thread started";
    });

    connect(thread,&QThread::finished,[](){
        qDebug() << "Thread finished";
    });


    connect(thread,&QThread::finished,thread,&QThread::deleteLater);

    thread->start();

    qDebug() << "Clicked on the start button";

}
```