---
title: "(Qt) Concurrent Map"
permalink: qt/mthread-ipc/concurrent-map/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/ConcurrentMap)

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
    static void modify(int & value);

private slots:
    void on_modifyButton_clicked();

    void on_seeValuesButton_clicked();

private:
    Ui::Widget *ui;
    QList<int> list;
    QFuture<void> future;
    QFutureWatcher<void> futureWatcher;
};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QThread>
#include <QDebug>
#include <QtConcurrent>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug() << "GUI Thread : " << QThread::currentThread();
    //Populate list
    for( int i {0} ;  i < 10000 ; i++){
        list << i;
    }


    qDebug() << "Original value : " << list.last();

    connect(&futureWatcher, &QFutureWatcher<void>::started,[=](){
        qDebug() << "asynchronous : Work started.";
    });

    connect(&futureWatcher, &QFutureWatcher<void>::finished,[=](){
        qDebug() << "asynchronous : Work done.";
    });

    connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged,[=](int value){
        qDebug() << "Progress : " << value;
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::modify(int &value)
{
    qDebug() << "Modifying " << value << " result : " << value * 10
             << " Thread :" << QThread::currentThread();
    value = value * 10;
}

void Widget::on_modifyButton_clicked()
{

    /*
    //Lambda function
    auto modify = [](int &value){
            qDebug() << "Modifying " << value << " result : " << value * 10
                     << " Thread :" << QThread::currentThread();
            value = value * 10;
        };
    */

    future = QtConcurrent::map(list,modify);

    futureWatcher.setFuture(future);
}

void Widget::on_seeValuesButton_clicked()
{
    qDebug() << "Modified value : " << list.last();
}
```