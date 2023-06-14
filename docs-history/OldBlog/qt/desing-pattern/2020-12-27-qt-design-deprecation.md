---
title: "(Qt) Code deprecation"
permalink: qt/desing-patterns/code-deprecation/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-27 00:00:00 -0000
last_modified_at: 2020-12-27 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Desing-Pattern
category:
  - Code deprecation
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

## Qt Code deprecation

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/2-11)

---

```cpp
#include <QCoreApplication>

/*

  What:
  code deprecation

  Description:
  Findings better ways to do things
  Some times you will want developers to use the better way

  Why:
  Can not remove the older function or it may break things
  But you want them to use the new way

  Example
  Q_DECL_DEPRECATED
  void original() Q_DECL_DEPRECATED;

 */


#include "machine.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Machine machine;
    machine.original();

    return a.exec();
}

```

```cpp
#ifndef MACHINE_H
#define MACHINE_H

#include <QObject>
#include <QDebug>

class Machine : public QObject
{
    Q_OBJECT
public:
    explicit Machine(QObject *parent = nullptr);

    //Old way of solving the problem
    void original() Q_DECL_DEPRECATED;

    //New way of solving the problem
    void newCode();

signals:

};

#endif // MACHINE_H

```

```cpp
#include "machine.h"

Machine::Machine(QObject *parent) : QObject(parent)
{

}

void Machine::original()
{
#warning Try the newCode function instead
#pragma message("You might still see this even if you are not using visual studio") //Visual Studio
    qInfo() << "Orginal code here";
}

void Machine::newCode()
{
    qInfo() << "New code here";
}

```