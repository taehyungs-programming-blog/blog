---
title: "(Qt) 16. QStrageInfo"
permalink: qt/core/QStrageInfo/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

## 전체코드

```cpp
#include <QCoreApplication>
#include <QStorageInfo>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    foreach(QStorageInfo storage, QStorageInfo::mountedVolumes()) {
        qInfo() << "Name: " << storage.displayName();
        qInfo() << "Type: " << storage.fileSystemType();
        qInfo() << "Total: " << storage.bytesTotal()/1000/1000 << "MB";
        qInfo() << "Available: " << storage.bytesAvailable()/1000/1000 << "MB";
        qInfo() << "Device: " << storage.device();
        qInfo() << "Root: " << storage.isRoot();
        qInfo("");
    }

    QStorageInfo root = QStorageInfo::root();
    QDir dir(root.rootPath());
    foreach(QFileInfo fi, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        qInfo() << fi.filePath();
    }

    return a.exec();
}
```