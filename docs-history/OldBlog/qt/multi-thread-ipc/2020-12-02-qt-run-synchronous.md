---
title: "(Qt) run synchronous"
permalink: qt/mthread-ipc/run-synchronous/                # link 직접 지정
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
  - Qt concurrent
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/RunSync)

- 우선 QFuture를 쓰긴하지만 메인 Thread에서 돌기에 Pause Cancel Resume등이 되지 않는다
- QFuture를 어떻게 쓰는지만 이해하자.

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFuture>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    static void heavyWork();

private slots:
    void on_startButton_clicked();

    void on_cancelButton_clicked();

    void on_pauseButton_clicked();

    void on_resumeButton_clicked();

private:
    Ui::Widget *ui;
    QFuture<void> future;
};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QtConcurrent>
#include <QThread>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug() << "Ui thread : " << QThread::currentThread();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::heavyWork()
{
    qDebug() << "Heavy work running in thread : " << QThread::currentThread();
    for(int i{0} ; i < 1000000001 ; i++){

        if((i%100000) == 0){

            double percentage = ((i/1000000000.0)) * 100;
                       qDebug() << "Percentage : " << QVariant::fromValue(percentage).toInt()
                                << " | Thread : " << QThread::currentThread();
        }
    }
}

void Widget::on_startButton_clicked()
{
   future = QtConcurrent::run(heavyWork);

   future.waitForFinished();

   qDebug() << "Computation done!";
}

void Widget::on_cancelButton_clicked()
{
    future.cancel();
}

void Widget::on_pauseButton_clicked()
{
    future.pause();
}

void Widget::on_resumeButton_clicked()
{
    future.resume();
}
```