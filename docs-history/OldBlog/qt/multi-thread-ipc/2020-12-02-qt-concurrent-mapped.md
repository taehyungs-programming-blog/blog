---
title: "(Qt) Concurrent Mapped"
permalink: qt/mthread-ipc/concurrent-mapped/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/ConcurrentMapped)

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

    static int modify(const int & value);

private slots:
    void on_modifyButton_clicked();

    void on_seeValuesButton_clicked();

private:
    Ui::Widget *ui;
    QList<int> list;
    QFuture<int> future;
    QFutureWatcher<int> futureWatcher;
};

#endif // WIDGET_H
```

widget.cpp

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
    qDebug() << "GUI Thread : " << QThread::currentThread();
    //Populate list
    for(int i {0} ;  i < 30000 ; i++){
        list << i;
    }


    //Monitor work using QFutureWatcher

    connect(&futureWatcher, &QFutureWatcher<void>::started,[=](){

        qDebug() << "asynchronous : Work started.";

    });

    connect(&futureWatcher, &QFutureWatcher<void>::finished,[=](){

        qDebug() << "asynchronous : Work finished.";

        qDebug() << "Result count : " << future.resultCount();

        //qDebug() << future.results();

        for( int i{0} ; i < future.resultCount(); i ++){
            qDebug() << "Result " << i << " :" << future.resultAt(i);
        }

    });
}

Widget::~Widget()
{
    delete ui;
}

int Widget::modify(const int &value)
{
    qDebug() << "Modifying " << value << " result : " << value * 10
             << " Thread :" << QThread::currentThread();
    return value * 10;
}

void Widget::on_modifyButton_clicked()
{

    /*
    std::function<int(const int &)> modify = [](const int & value){
        qDebug() << "Modifying " << value << " result : " << value * 10
                 << " Thread :" << QThread::currentThread();
        return value * 10;
    };

    */


    //Synchronous
    future = QtConcurrent::mapped(list,modify);

    future.waitForFinished();

    qDebug() << "asynchronous : Work finished.";

    qDebug() << "Result count : " << future.resultCount();

    //qDebug() << future.results();

    for( int i{0} ; i < future.resultCount(); i ++){
        qDebug() << "Result " << i << " :" << future.resultAt(i);
    }



    //futureWatcher.setFuture(future);
}

void Widget::on_seeValuesButton_clicked()
{
    qDebug() << "Original list " << list;
}
```