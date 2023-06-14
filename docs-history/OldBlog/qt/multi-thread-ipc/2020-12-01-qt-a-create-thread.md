---
title: "(Qt) QThread asynchronous(Create Thread)"
permalink: qt/mthread-ipc/a-create-thread/                # link 직접 지정
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
  - asynchronous
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/AsyncCreatThread)

Thread의 생성을 비동기(임의로) 적으로 해보자.

---

## 전체코드

Widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include   <QThread>

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
    QThread * thread;
};

#endif // WIDGET_H
```

Widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QTimer>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug() << "Ui thread : " << QThread::currentThread();

    thread = QThread::create([](){

        QTimer * timer = new QTimer();
        connect(timer,&QTimer::timeout,[](){
            qDebug() << "Time out . Running in thread : " << QThread::currentThread();
        });
        timer->start(1000);

    });

    /*
    // 만약 외부에 timer가 있을경우 메인 thread에서 실행이 됨.
    QTimer * timer = new QTimer();
        connect(timer,&QTimer::timeout,[](){
            qDebug() << "Time out . Running in thread : " << QThread::currentThread();
        });
    timer->start(1000);
    */

    connect(thread,&QThread::started,[](){
        qDebug() << "Thread started";
    });

    connect(thread,&QThread::finished,[](){
        qDebug() << "Thread finished";
    });

  //  thread->start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{
    thread->start();
}
```