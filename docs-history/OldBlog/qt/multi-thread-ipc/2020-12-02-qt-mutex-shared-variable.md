---
title: "(Qt) Mutex Shared Variable"
permalink: qt/mthread-ipc/mutex-shared-variable/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/MutexSharedVariable)

간단하다.. mutex locker만들고 count관리

---

## 전체코드

main.cpp

```cpp
#include <QApplication>
#include <QMessageBox>
#include "printdevice.h"
#include "printworker.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    bool stopFlag = false;

    PrintDevice printDevice;

    PrintWorker black("Black",&stopFlag,&printDevice),white("White",&stopFlag,&printDevice);

    black.start();
    white.start();

    QMessageBox::information(nullptr,"QMutex",
                                 "Thread working. Close Me to stop");

    stopFlag = true;

    black.wait();
    white.wait();


    return 0;
}
```

printworker.h

```cpp
#ifndef PRINTWORKER_H
#define PRINTWORKER_H

#include <QThread>
#include "printdevice.h"

class PrintWorker : public QThread
{
    Q_OBJECT
public:
    explicit PrintWorker(QString name, bool * stop, PrintDevice * worker,QObject *parent = nullptr);

signals:

public slots:
private:
    QString m_name;
    bool * m_stop;
    PrintDevice * m_print_device;

    // QThread interface
protected:
    void run() override;
};

#endif // PRINTWORKER_H
```

printworker.cpp

```cpp
#include "printworker.h"

PrintWorker::PrintWorker(QString name, bool * stop, PrintDevice * printDevice,QObject *parent)
    : QThread(parent),
      m_name(name), m_stop(stop),m_print_device(printDevice)
{

}
void PrintWorker:: run() {
    while(!*m_stop){

        m_print_device->print(m_name);
        sleep(1);

    }
}
```

printdevice.h

```cpp
#ifndef PRINTDEVICE_H
#define PRINTDEVICE_H

#include <QString>
#include <QMutex>

class PrintDevice
{
public:
    PrintDevice();

    void print(const QString & text);

private:
    int m_count;
    QMutex m_mutex;
};

#endif // PRINTDEVICE_H
```

printdevice.cpp

```cpp
#include "printdevice.h"
#include <QDebug>
#include <QMutexLocker>

PrintDevice::PrintDevice()
{
    m_count = 0;
}

void PrintDevice::print(const QString &text)
{
    QMutexLocker locker(&m_mutex);
    qDebug() << text << "Called , count : " << m_count++ ;
}
```