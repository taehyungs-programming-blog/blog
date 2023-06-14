---
title: "(Qt) 30. Compressing data"
permalink: qt/core/compress-data/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci7-1)

---

```cpp
#include <QCoreApplication>
#include <QDebug>

QByteArray makeData() {
    QByteArray data;

    for(int i = 0; i < 1000; i++) {
        data.append(QString::number(i) + "\r\n");
    }

    return data;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QByteArray data = makeData();
    QByteArray compressed = qCompress(data);
    QByteArray decompressed = qUncompress(compressed);

    qInfo() << "Original: " << data.length();
    qInfo() << "Compressed: " << compressed.length();
    qInfo() << "Decompressed: " << decompressed.length();

    return a.exec();
}
```