---
title: "(Qt) library patterns"
permalink: qt/desing-patterns/library/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-31 00:00:00 -0000
last_modified_at: 2020-12-31 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Desing-Pattern
category:
  - library patterns
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

## Creating a shared lib

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/14-1)

```cpp
#ifndef UPLOADER_GLOBAL_H
#define UPLOADER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UPLOADER_LIBRARY)
#  define UPLOADER_EXPORT Q_DECL_EXPORT
#else
#  define UPLOADER_EXPORT Q_DECL_IMPORT
#endif

#endif // UPLOADER_GLOBAL_H
```

```cpp
#ifndef UPLOADER_H
#define UPLOADER_H

#include "uploader_global.h"
#include <QDebug>
#include <QString>
#include <QTcpSocket>

class UPLOADER_EXPORT Uploader
{
public:
    Uploader();
    void upload(QString filename);

    QTcpSocket *socket() const;
    void setSocket(QTcpSocket *socket);

private:
    QTcpSocket *m_socket;

};

#endif // UPLOADER_H
```

---

## Using a shared lib

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/14-2)

```cpp
#include <QCoreApplication>

/*

  What
  Using a shared lib

  Description
  How to use a shared lib

  Why
  Someone else created the lib or its not a part of this project

  Example
  Add external lib from 14-2

  Special notice to debug vs release!!!
myapp$ ldd ./myapp
    libuploader.so.1 => not found

    Talk about qt depoloy
    And the lib needs to be distributed with the app
    https://doc.qt.io/qt-5/deployment.html

 */
#include "../qt5dp-14-1/uploader/uploader.h"
#include "../qt5dp-14-1/uploader/uploader_global.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Uploader up;
    up.upload("test.txt");

    return a.exec();
}
```

--

## lib dependency tree

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/14-3)

---

## loading lib dynamically

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/14-4)

```cpp
#include <QCoreApplication>

/*
 What
 Use a lib without creating a dependacy tree

 Description
 We dont want to enforce linking to QtNetwork

 Why
 Because we want to load this lib dynamically

 Example
 Loading a lib
    https://doc.qt.io/qt-5/qlibrary.html#resolve

    WHAT A PAIN
 */


#include <QDebug>
#include <QLibrary>

bool loadlib(QLibrary *lib, QString path)
{
    lib->setFileName(path);
    lib->setLoadHints(QLibrary::ResolveAllSymbolsHint);

    if(!lib->load())
    {
        qWarning() << "Failed to load the lib!";
        qWarning() << lib->errorString();
        return false;
    }

    return lib->isLoaded();

}

void callFunction(QLibrary *lib)
{
    double value = -1;
    typedef int (*AvgFunction)(int, int);
    AvgFunction avg = (AvgFunction) lib->resolve("avg");

    if(avg) value = avg(5,8);
    qInfo() << value;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString path = "/home/rootshell/Code/Qt/build-qt5dp-14-4-Desktop_Qt_5_12_3_GCC_64bit-Debug/bla/libbla.so.1.0.0";
    QLibrary lib;

    //load
    if(loadlib(&lib,path))
    {
        //use
        callFunction(&lib);

        //unload the lib when we are done
        lib.unload(); //will return false if being used by something else!
    }

    return a.exec();
}
```

---

## Plug-in

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/14-6)

