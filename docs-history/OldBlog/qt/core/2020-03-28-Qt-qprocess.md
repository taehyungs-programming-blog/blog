---
title: "(Qt) 42. QProcess"
permalink: qt/core/qprocess/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci10-2)

---

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>

bool test() {
    QProcess gzip;
    gzip.start("gzip", QStringList() << "-c");

    if(!gzip.waitForStarted()) return false;

    gzip.write("Qt rocks!");
    gzip.closeWriteChannel();

    if(!gzip.waitForFinished()) return false;
    QByteArray result = gzip.readAll();
    qInfo() << "Result: " << result;

    return true;

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(test()) {
        qInfo() << "Compressed with gzip!";
    } else {
        qInfo() << "Failed to use gzip";
    }

    return a.exec();
}
```