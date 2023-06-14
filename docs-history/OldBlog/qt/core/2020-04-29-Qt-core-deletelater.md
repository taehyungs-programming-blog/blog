---
title: "Qt - Core : delete later"
permalink: qt/core/deletelater/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-29 00:00:00 -0000
last_modified_at: 2020-04-29 00:00:00 -0000
sidebar:
  title: "Qt"
  nav: qt
---

* Qt Message Queue에 넣어 Object Delete를 수행한다.

```cpp
void MyClass::deferLongComputation()
{
    QTimer* timer = new QTimer();

    connect(timer, 
            &QTimer::timeout,
            [this, timer](){this->longComputiation(); timer->deleteLater();});

    timer->setSingleShot(true);
    timer->start();
}
```