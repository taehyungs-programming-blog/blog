---
title: "(Qt) 32. Serializing QObjects"
permalink: qt/core/serializing-qobject/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci8-1)

---

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QDataStream>
#include <QFile>

//Serialization

bool saveFile(QString path) {
    QFile file(path);

    if(!file.open(QIODevice::WriteOnly)) return false;

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_11);

    QString title = "The answer is 42";
    qint64 num = 42;

    out << title;
    out << num;

    file.flush();
    file.close();

    return true;
}

bool readFile(QString path) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) return false;

    QDataStream in(&file);
    if(in.version() != QDataStream::Qt_5_11) {
        qCritical() << "Bad version!";
        file.close();
        return false;
    }

    QString title;
    qint64 num;


    in >> title;
    in >> num;

    qInfo() << title;
    qInfo() << num;

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString file = "test.txt";

    if(saveFile(file)) {
        qInfo() << "Saved!";
        readFile(file);
    }

    return a.exec();
}
```