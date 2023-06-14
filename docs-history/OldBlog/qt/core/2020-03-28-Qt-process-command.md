---
title: "(Qt) 44. QProcess - command line"
permalink: qt/core/process-command/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci10-4)

---

```cpp
#include <QCoreApplication>
#include "commander.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    commander cmd;
    cmd.action(QByteArray("pwd"));
    cmd.action(QByteArray("ls -l"));

    // system("pwd")
    // system("ls -l")

    return a.exec();
}
```

```cpp
// commander.h
#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include <QDebug>
#include <QProcess>

class commander : public QObject
{
    Q_OBJECT
public:
    explicit commander(QObject *parent = nullptr);
    ~commander();
signals:

public slots:
    void readyRead();
    void action(QByteArray data);

private:
    QProcess proc;
    QString app;
    QString endl;
};

#endif // COMMANDER_H
```

```cpp
// commander.cpp
#include "commander.h"

commander::commander(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_WIN
    //Windows
    app = "cmd";
    endl = "\r\n";
#else
    //Not windows
    app = "bash";
    endl = "\n";
#endif

    proc.setProgram(app);
    proc.start();
    proc.setReadChannelMode(QProcess::MergedChannels);
    connect(&proc,&QProcess::readyRead, this, &commander::readyRead);
    connect(&proc,&QProcess::readyReadStandardOutput, this, &commander::readyRead);
    connect(&proc,&QProcess::readyReadStandardError, this, &commander::readyRead);

}

commander::~commander()
{
    if(proc.isOpen()) proc.close();
}

void commander::readyRead()
{
    qint64 value = 0;
    do {
        QByteArray line = proc.readAll();
        qInfo() << line.trimmed();
        value = line.length();
    }while(value > 0);
}

void commander::action(QByteArray data)
{
    if(!data.endsWith(endl.toLatin1())) data.append(endl);
    proc.write(data);
    proc.waitForBytesWritten(1000);
}
```