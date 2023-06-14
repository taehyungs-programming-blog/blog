---
title: "(Qt) 43. Exit codes"
permalink: qt/core/exitcodes/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-28 00:00:00 -0000
last_modified_at: 2020-03-28 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

> * [Github](https://github.com/8bitscoding/qtci10-3)

---

```cpp
#include <QCoreApplication>
#include <QDebug>
#include<QProcess>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInfo() << "Starting...";
    QProcess proc;
    proc.execute("xed",QStringList() << "http://www.voidrealms.com");
    qInfo() << "Exit code: " << proc.exitCode();

    return a.exec();
}
```