---
title: "(Qt) 29. QTextCodec"
permalink: qt/core/qtextcodec/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci6-5)

---

```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QTextCodec>

//Special Codec
//http://doc.qt.io/qt-5/qtextcodec.html

QString makeData() {
    QString data;
    for(int i = 0; i < 10; i++) {
        data.append("Hello\r\n");
    }
    return  data;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString data = makeData();
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    if(!codec) qFatal("No codec!");

    QByteArray bytes = codec->fromUnicode(data);
    qInfo() << "Bytes: " << bytes;
    qInfo() << "-----------------------------------------";
    QString string = codec->toUnicode(bytes);
    qInfo() << "String: " << string;


    return a.exec();
}
```