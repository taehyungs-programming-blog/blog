---
title: "(Qt) QThread asynchronous(Subclass QThread)"
permalink: qt/mthread-ipc/a-subclass-thread/                # link 직접 지정
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
  - subclass-thread
  - asynchronous
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/AsyncSubclass-QThread)

---

## 전체코드

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "workerthread.h"

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

    void on_stopButton_clicked();

    void on_infoButton_clicked();

private:
    Ui::Widget *ui;
    WorkerThread * thread;
};

#endif // WIDGET_H
```

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    thread = new WorkerThread(this);

    connect(thread,&WorkerThread::finished,[](){
        qDebug() << "Thread finished";
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{
    thread->start();
}

void Widget::on_stopButton_clicked()
{
    thread->exit();

}

void Widget::on_infoButton_clicked()
{
   qDebug() << "Thread running : " << thread->isRunning();
}
```