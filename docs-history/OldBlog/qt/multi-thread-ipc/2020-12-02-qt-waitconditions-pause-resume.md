---
title: "(Qt) WaitConditions Pause Resume"
permalink: qt/mthread-ipc/waitconditions-pause-resume/                # link 직접 지정
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
  - WaitConditions
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/WaitConditionsPauseResume)

---

## 전체코드

widget.h

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

    void on_pauseButton_clicked();

    void on_resumeButton_clicked();

    void currentCount(int value);

private:
    Ui::Widget *ui;
    WorkerThread * workerThread;
};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug() << "Ui thread : " << QThread::currentThread();

    workerThread = new WorkerThread(this);
    connect(workerThread,&WorkerThread::currentCount,this,&Widget::currentCount);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{
    workerThread->start();
}

void Widget::on_pauseButton_clicked()
{
    workerThread->pause();
}

void Widget::on_resumeButton_clicked()
{
    workerThread->resume();
}

void Widget::currentCount(int value)
{
    ui->progressBar->setValue(value);
}
```

workerthread.h

```cpp
#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThread(QObject *parent = nullptr);
    ~WorkerThread() override;

signals:
    void currentCount(int value);

public slots:
    void pause();
    void resume();


    // QThread interface
protected:
    void run() override;

private:
    QMutex m_mutex;
    QWaitCondition m_pause_condition;
    bool m_pause;

};

#endif // WORKERTHREAD_H
```

workerthread.cpp

```cpp
#include "workerthread.h"
#include <QDebug>

WorkerThread::WorkerThread(QObject *parent) : QThread(parent)
{
    m_pause = false;
    qDebug() << "WorkerThread object constructed thread : " << QThread::currentThread();

}

WorkerThread::~WorkerThread()
{
    qDebug() << "WorkerThread object destroyed thread : " << QThread::currentThread();
}

void WorkerThread::pause()
{
    qDebug() << "Pause method running in thread : " << QThread::currentThread();
    m_mutex.lock();

    m_pause = true;
    m_mutex.unlock();

}

void WorkerThread::resume()
{
    qDebug() << "Resume method running in thread : " << QThread::currentThread();

    m_mutex.lock();

    m_pause = false;

    m_mutex.unlock();
    m_pause_condition.wakeAll();


}

void WorkerThread::run()
{
    qDebug() << "Run method running in thread : " << QThread::currentThread();


    for(int i{0} ; i < 1000000001 ; i++){


        m_mutex.lock();

        if(m_pause)
            m_pause_condition.wait(&m_mutex);//Freeze code execution in this thread

        m_mutex.unlock();

        /*
                 * Only emit signal to send info to ui at 100000 intervals. UI can handle this.
                 * Otherwise it is going to freeze.
                 * */
        if((i%100000) == 0){
            double percentage = ((i/1000000000.0)) * 100;
            emit currentCount(QVariant::fromValue(percentage).toInt());
        }
    }
}
```