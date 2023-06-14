---
title: "(Qt) 35. Serializing to XML"
permalink: qt/core/serializing-xml/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci8-4)

---

```cpp
#include <QCoreApplication>
#include <QDebug>
#include "test.h"
#include "converter.h"


void writeJson(QString path) {
    test t;
    t.fill();
    converter::writeJson(&t, path);
}

void readJson(QString path) {
    test *t;
    t = converter::readJson(path);

    if(!t) {
        qInfo() << "Object not loaded";
        return;
    }

    qInfo() << "Name: " << t->name();
    foreach(QString key, t->map().keys()) {
        qInfo() << key << " : " << t->map().value(key);
    }
}

void writeXml(QString path) {
    test t;
    t.fill();
    converter::writeXml(&t, path);
}

void readXml(QString path) {
    test *t;
    t = converter::readXml(path);

    if(!t) {
        qInfo() << "Object not loaded";
        return;
    }

    qInfo() << "Name: " << t->name();
    foreach(QString key, t->map().keys()) {
        qInfo() << key << " : " << t->map().value(key);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString xmlpath = "test_xml.txt";
    QString jsonpath = "test_json.txt";

    writeJson(jsonpath);
    readJson(jsonpath);

    writeXml(xmlpath);
    readXml(xmlpath);

    return a.exec();
}
```