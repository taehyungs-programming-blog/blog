---
title: "(Qt) 15. QFileInfo"
permalink: qt/core/QFileInfo/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

## 사용법

```cpp
QDir dir;

QFileInfoList files = dir.entryInfoList(QDir::Files);

foreach(QFileInfo fi, files) {
    qInfo() << "..Name: " << fi.fileName();
    qInfo() << "...Size: " << fi.size();
    qInfo() << "...Created: " << fi.birthTime();
    qInfo() << "...Modified: " << fi.lastModified();
}
```

---

## 전체코드

```cpp
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
#include <QDebug>


void list(QString path) {
    qInfo() << "Listing: " << path;

    QDir dir(path);
    qInfo() << dir.absolutePath();

    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList files = dir.entryInfoList(QDir::Files);

    foreach(QFileInfo fi, dirs) {
        qInfo() << fi.fileName();
    }

    foreach(QFileInfo fi, files) {
        qInfo() << "..Name: " << fi.fileName();
        qInfo() << "...Size: " << fi.size();
        qInfo() << "...Created: " << fi.birthTime();
        qInfo() << "...Modified: " << fi.lastModified();

    }

    foreach(QFileInfo fi, dirs) {
        list(fi.absoluteFilePath());
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    list(QDir::tempPath());


    return a.exec();
}
```