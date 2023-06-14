---
title: "(Qt) Asynchronous Return values"
permalink: qt/mthread-ipc/async-return-val/                # link 직접 지정
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

- `QFutureWatcher`를 통해 어떻게 `QFuture`를 사용하는지
- return value를 어떻게 받아오는지
- 일종에 thread를 나누는 개념이다.

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFuture>
#include <QFutureWatcher>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

  static  QFuture<QVector<int>> heavyWorkWithReturn();

private slots:
    void on_startButton_clicked();

    void on_cancelButton_clicked();

    void on_pauseButton_clicked();

    void on_resumeButton_clicked();

private:
    Ui::Widget *ui;
    QFuture<QVector<int>> future;
    QFutureWatcher<QVector<int>> watcher;
};

#endif // WIDGET_H
```

widget.h

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QThread>
#include <QtConcurrent>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << "Ui thread : " << QThread::currentThread();

    connect(&watcher,&QFutureWatcher<void>::finished,[=](){
        qDebug() << "---------------------------Computation done----------------------";


        if(!watcher.isCanceled()){

            QVector<int> resultVector = watcher.result();

            qDebug() << "Number of items in resultVector : " << resultVector.count();

            for( int i{0}; i < resultVector.count(); i++){
                qDebug() << " element [" << i << "] : " << resultVector[i];
            }

        }else{
            qDebug() << "Watcher already canceled ";
        }
    });
}

Widget::~Widget()
{
    delete ui;
}

QFuture<QVector<int> > Widget::heavyWorkWithReturn()
{
    auto heavyWork = [=](){

        qDebug() << "Heavy work running in thread : " << QThread::currentThread();

        QVector<int> intVector;

        for(int i{0} ; i < 1000000001 ; i++){

            if((i%100000) == 0){
                double percentage = ((i/1000000000.0)) * 100;
                qDebug() << "Heavy work with return : " << QVariant::fromValue(percentage).toInt() ;
                intVector.push_back(QVariant::fromValue(percentage).toInt());
            }
        }
        return  intVector;
    };

    return  QtConcurrent::run(heavyWork);
}

void Widget::on_startButton_clicked()
{
    future =  heavyWorkWithReturn();

    watcher.setFuture(future);
}

void Widget::on_cancelButton_clicked()
{
    watcher.cancel();
}

void Widget::on_pauseButton_clicked()
{
    watcher.pause();
}

void Widget::on_resumeButton_clicked()
{
    watcher.resume();
}
```
