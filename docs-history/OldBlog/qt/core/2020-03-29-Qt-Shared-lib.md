---
title: "(Qt) 37. Shared library"
permalink: qt/core/shared-lib/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-29 00:00:00 -0000
last_modified_at: 2020-03-29 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtca-3-1)

```cpp
#include <QCoreApplication>

/*
Add this to the project file to get rid of the compile issues
CONFIG+=ordered
*/

#include "../mylib/mylib_global.h"
#include "../mylib/mylib.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Mylib mylib;
    mylib.test();

    //some change

    return a.exec();
}
```

```cpp
#ifndef MYLIB_GLOBAL_H
#define MYLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MYLIB_LIBRARY)
#  define MYLIBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MYLIBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MYLIB_GLOBAL_H

```

```cpp
#ifndef MYLIB_H
#define MYLIB_H

#include "mylib_global.h"
#include <QDebug>

class MYLIBSHARED_EXPORT Mylib
{

public:
    Mylib();
    void test();
};

#endif // MYLIB_H
```

```cpp
#include "mylib.h"

Mylib::Mylib()
{
}

void Mylib::test()
{
    qInfo() << "This is a test from my lib!";
}

```