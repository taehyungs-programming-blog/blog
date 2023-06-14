---
title: "(Qt) 19. QDataStream"
permalink: qt/core/QDataStream/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QDataStream>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>

bool createFile(QString path) {
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) {
        qWarning() << file.errorString();
        return false;
    }

    QDataStream stream(&file);
    int max = 1000;
    QString banner = "Random number:";
    qInfo() << "Writing " << banner;
    stream << banner;

    for(int i = 0; i < 5; i++) {
        qint32 num = QRandomGenerator::global()->bounded(max);
        qInfo() << "Writing: " << num;
        stream << num;
    }

    file.close();

    return true;
}

void readFile(QString path) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << file.errorString();
        return;
    }

    QDataStream stream(&file);

    QString banner;
    stream >> banner;

    qInfo() << "Banner: " << banner;

    for(int i = 0; i < 5; i++) {
       qint32 num;
       stream >> num;
       qInfo() << "Random: " << num;
    }

    file.close();
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString path = QDir::currentPath() + QDir::separator() + "test.txt";
    qInfo() << "Path: " << path;
    if(createFile(path)) {
        readFile(path);
    }

    return a.exec();
}
```