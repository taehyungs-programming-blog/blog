---
title: "(Qt) 45. QTimer - single shot"
permalink: qt/core/single-shot/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

```cpp
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

void test() {
    qInfo() << "Thank you for waiting!";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInfo() << "Please wait...";

    QTimer::singleShot(5000,&test);

    return a.exec();
}
```