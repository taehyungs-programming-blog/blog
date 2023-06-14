---
title: "(Qt) ReadWrite Lock"
permalink: qt/mthread-ipc/readwrite-lock/                # link 직접 지정
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
  - mutex
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/SyncReadWrite)

Mutex를 이용하여 Timing을 조절한다.

---

## 전체코드

main.cpp

```cpp
#include <QApplication>
#include <QMessageBox>
#include "printdevice.h"
#include "printworker.h"
#include "incrementworker.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool stopFlag = false;

    PrintDevice printDevice;

    PrintWorker white("White",&stopFlag,&printDevice),black("Black",&stopFlag,&printDevice);

    IncrementWorker incrementWorker(&stopFlag,&printDevice);


    white.start();
    black.start();
    incrementWorker.start();

    QMessageBox::information(nullptr,"QMutex",
                                 "Thread working. Close Me to stop");

    stopFlag = true;

    white.wait();
    black.wait();
    incrementWorker.wait();

    return 0;
}
```

incrementworker.h

```cpp
#ifndef INCREMENTWORKER_H
#define INCREMENTWORKER_H

#include <QThread>
#include "printdevice.h"

class IncrementWorker : public QThread
{
    Q_OBJECT
public:
    explicit IncrementWorker(bool * stop,PrintDevice * printDevice,QObject *parent = nullptr);

signals:

public slots:

private:
    PrintDevice * m_print_device;
    bool * m_stop ;

    // QThread interface
protected:
    void run() override;
};

#endif // INCREMENTWORKER_H
```

incrementworker.cpp

```cpp
#include "incrementworker.h"

IncrementWorker::IncrementWorker(bool * stop,PrintDevice * printDevice,
                                 QObject *parent) : QThread(parent),
    m_print_device(printDevice),
    m_stop(stop)
{

}

void IncrementWorker::run()
{
        while (!(*m_stop)) {
            msleep(1500);
            m_print_device->increment();
        }
}
```

printdevice.cpp

```cpp
#include "printdevice.h"
#include <QDebug>

PrintDevice::PrintDevice()
{
    m_count = 0;
}


void PrintDevice::print(const QString  & text){

    QReadLocker readLocker(&m_read_write_lock);
    qDebug() << text << "Called , count : " << m_count ;

}

void PrintDevice::increment(){
    QWriteLocker writeLocker(&m_read_write_lock);
    m_count++;
}
```