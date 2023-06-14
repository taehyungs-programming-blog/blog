---
title: "(Qt) 13. QBuffer"
permalink: qt/core/QBuffer/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

## 사용하기

```cpp
QBuffer buffer;

if(buffer.open(QIODevice::ReadWrite)) {
    buffer.write(data);
    buffer.close();
}
```

---

## 전체코드

```cpp
#include <QCoreApplication>
#include <QIODevice>
#include <QBuffer>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QBuffer buffer;

    if(buffer.open(QIODevice::ReadWrite)) {
        qInfo() << "Buffer Opened!";
         QByteArray data("Hello World");

         for(int i = 0; i < 5; i++) {
             buffer.write(data);
             buffer.write("\r\n");
         }
         //File and device access you may need to flush the data to the device
         //buffer.flush()

         //Move to the first position
         buffer.seek(0);

         qInfo() << buffer.readAll();

         qInfo() << "closing the buffer";

         //ALWAYS close your device!
         buffer.close();


    } else {
        qInfo() << "Could not open buffer!!!";
    }


    return a.exec();
}
```