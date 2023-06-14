---
title: "(Qt) Concurrent filtered"
permalink: qt/mthread-ipc/concurrent-filtered/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/ConcurrentFiltered)

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

private slots:
    void on_filterButton_clicked();

    void on_filterSpinBox_valueChanged(int arg1);

private:
    Ui::Widget *ui;
    int filterValue;
    QList<int> intList;
    QFuture<int> future;
    QFutureWatcher<int> futureWatcher;
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
    qDebug() << "Ui thread : " <<QThread::currentThread();

    //Populate int list with numbers
    for(int i{0} ; i < 100 ; i++){
        intList << QRandomGenerator::global()->bounded(1000);
    }

    qDebug() << "Before , item count : " << intList.count();

    //Clear the lists
    ui->originalList->clear();
    ui->filteredList->clear();


    //Populate original ui listWidget
    foreach (int value, intList) {
        ui->originalList->addItem(QString::number(value));
    }

    //Monitor work using QFutureWatcher

    connect(&futureWatcher, &QFutureWatcher<void>::started,[=](){

        qDebug() << "asynchronous : Work started.";

    });

    connect(&futureWatcher, &QFutureWatcher<void>::finished,[=](){

        qDebug() << "asynchronous : Work finished.";

        //Get the results and use them
        QList<int> newList = future.results();

        qDebug() << "After , item count : " << newList.count();

        foreach (int value, newList) {
            ui->filteredList->addItem(QString::number(value));
        }


    });

    filterValue = ui->filterSpinBox->value();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_filterButton_clicked()
{

    auto filter = [=](const int value){

        if(value >= filterValue){
            qDebug() << "Thread " << QThread::currentThread() << ". Value " << value << " greater that threshold " <<filterValue << " filtering out.";
            return false;
        }
        return true;
    };


    //Clear filtered list
    ui->filteredList->clear();

    future = QtConcurrent::filtered(intList,filter);

    future.waitForFinished();

    qDebug() << "synchronous : Work finished.";

    //Get the results and use them
    QList<int> newList = future.results();

    qDebug() << "After , item count : " << newList.count();

    foreach (int value, newList) {
        ui->filteredList->addItem(QString::number(value));
    }

   // futureWatcher.setFuture(future);

}

void Widget::on_filterSpinBox_valueChanged(int arg1)
{
    filterValue = arg1;
}
```