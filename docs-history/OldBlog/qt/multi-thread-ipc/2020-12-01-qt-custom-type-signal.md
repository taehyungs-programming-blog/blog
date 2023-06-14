---
title: "(Qt) Custom Type Signal Parameters"
permalink: qt/mthread-ipc/custom-type-signal/                # link 직접 지정
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
  - signal
  - custom
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/CustomTypeSignals)

Signal / Slot을 어떻게 쓰는지에 대해서 인데 `Q_DECLARE_METATYPE(NumberedString)`를 선언해야 매개변수로 Numberstring(사용자가 만든 오브젝트)를 쓸수 있다는 점을 기억하자

---

## 전체코드

Widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include "numberedstring.h"

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

    void on_infoButton_clicked();

    void countDone();

    void currentCount(int value);

    void threadFinished();

    void currentNumberString(NumberedString ns);

private:
    Ui::Widget *ui;
    QThread * workerThread;
};

#endif // WIDGET_H
```

Widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include "worker.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << "Widget Constructor running in GUI thread : " << thread();
    qDebug() << "Current thread in Widget constructor : " << QThread::currentThread();

    workerThread = new QThread(this);
    connect(workerThread,&QThread::finished,this,&Widget::threadFinished);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{

    Worker * worker = new Worker;//Main thread

    worker->moveToThread(workerThread);

    connect(workerThread,&QThread::started,worker,&Worker::doCounting);
    connect(worker,&Worker::currentNumberString,this,&Widget::currentNumberString);
    connect(worker,&Worker::countDone,this,&Widget::countDone);
    connect(workerThread,&QThread::finished,worker,&Worker::deleteLater);

    workerThread->start();

}

void Widget::on_infoButton_clicked()
{
    qDebug() << " Thread running : " << workerThread->isRunning();
}

void Widget::countDone()
{
    qDebug() << "Widget, count Done";
    workerThread->exit();
}

void Widget::currentCount(int value)
{
    ui->progressBar->setValue(value);
    ui->infoLabel->setText(QString::number(value));

}

void Widget::threadFinished()
{
    qDebug() << "Thread finished. Thread : " << thread();
    qDebug() << "Thread finished.Current Thread : " << QThread::currentThread();
}

void Widget::currentNumberString(NumberedString ns)
{
    ui->progressBar->setValue(ns.number());
    ui->infoLabel->setText(ns.string());
}
```

Numberedstring.h

```cpp
#ifndef NUMBEREDSTRING_H
#define NUMBEREDSTRING_H

#include <QMetaType>
#include <QString>

class NumberedString
{
public:
    NumberedString();
    NumberedString(int number , QString string);

    int number() const;

    QString string() const;

private:
    int m_number;
    QString m_string;
};
Q_DECLARE_METATYPE(NumberedString)

#endif // NUMBEREDSTRING_H
```

Numberedstring.cpp

```cpp
#include "numberedstring.h"

NumberedString::NumberedString()
{

}

NumberedString::NumberedString(int number, QString string) :
m_number(number), m_string(string)
{

}

int NumberedString::number() const
{
    return m_number;
}

QString NumberedString::string() const
{
    return m_string;
}
```

Worker.h

```cpp
#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include "numberedstring.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

signals:
    void currentCount(int value);
    void currentNumberString(NumberedString ns);
    void countDone();
public slots:
    void doCounting();
};

#endif // WORKER_H
```

Worker.cpp

```cpp
#include "worker.h"
#include <QVariant>
#include <QThread>
#include <QDebug>

Worker::Worker(QObject *parent) : QObject(parent)
{
    qDebug() << "Worker constructor running in thread : " << QThread::currentThread();
}

Worker::~Worker()
{
    qDebug() << "Worker destroyed, destructor running in thread : " << QThread::currentThread();
}

void Worker::doCounting()
{

    qDebug() << "Worker counting method running in thread : " << QThread::currentThread();
    //Code here is going to run in secondary/background thread

    for(int i{0} ; i < 1000000001 ; i++){

        double percentage = (i/ 1000000000.0) * 100.0;

        //qDebug() << "Percentage : " << percentage;

        if((i% 100000) ==0){

            QString txt = "Hello " +QString::number(QVariant::fromValue(percentage).toInt());

            NumberedString ns(QVariant::fromValue(percentage).toInt(),txt);

            emit currentNumberString(ns);
        }
    }
    emit countDone();
}
```