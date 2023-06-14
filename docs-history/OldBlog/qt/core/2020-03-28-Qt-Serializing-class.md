---
title: "(Qt) 33. Serializing a class"
permalink: qt/core/serializing-class/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci8-2)

---

```cpp
#include <QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include "test.h"

bool saveFile(test *t,QString path) {
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly)) return false;

    QDataStream ds(&file);
    ds.setVersion(QDataStream::Qt_5_11);

    //ds << t->name();
    // ds << t->map();

    ds << *t;

    file.close();
    return true;
}

bool loadFile(QString path) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)) return false;

    QDataStream ds(&file);
    test t;
    ds >> t;

    file.close();

    qInfo() << "Name: " << t.name();
    foreach(QString key, t.map().keys()) {
        qInfo() << key << " : " << t.map().value(key);
    }

    return true;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString path = "test.txt";

    test t;
    t.fill();
    if(saveFile(&t,path)) {
        loadFile(path);
    }

    return a.exec();
}
```

```cpp
// test.h
#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QDebug>
#include <QMap>
#include <QDataStream>

class test : public QObject
{
    Q_OBJECT
public:
    explicit test(QObject *parent = nullptr);

    void fill();
    QString name();
    void setName(QString value);
    QMap<QString, QString> map();

    friend QDataStream& operator <<(QDataStream &stream, const test &t) {
        qInfo() << "Operload <<";
        stream << t.m_name;
        stream << t.m_map;
        return stream;
    }

    friend QDataStream& operator >>(QDataStream &stream, test &t) {
        qInfo() << "Operload >>";
        stream >> t.m_name;
        stream >> t.m_map;
        return stream;
    }

signals:

public slots:

private:
    QString m_name;
    QMap<QString, QString> m_map;
};

#endif // TEST_H
```

```cpp
// test.cpp
#include "test.h"

test::test(QObject *parent) : QObject(parent)
{

}

void test::fill()
{
    m_name = "test object";
    for(int i = 0; i < 10; i++) {
        QString num = QString::number(i);
        QString key = "Key: " + num;
        QString value = "This is item number: " + num;
        m_map.insert(key,value);
    }
}

QString test::name() {
    return m_name;
}

void test::setName(QString value){
    m_name = value;
}

QMap<QString,QString> test::map() {
    return  m_map;
}
```