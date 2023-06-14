---
title: "(Qt) Template Patterns"
permalink: qt/desing-patterns/template/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-12 00:00:00 -0000
last_modified_at: 2021-01-12 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Desing-Pattern
category:
  - Template Patterns
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

## 7-1

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/7-1)

별거없음 설명생략

---

## 7-2

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/7-2)

별거없음 설명생략

---

## 7-3

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/7-3)

`QObject`임을 확인

```cpp
template <class T>
void testQObject(T &x)
{
    if(!std::is_base_of<QObject, T>::value)
    {
        qInfo() << "Not a QObject";
        return;
    }

    x.setObjectName("Hello from template");
}
```

---

## 7-4

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/7-4)

```cpp
void printList(QList<MyClass*> &list)
{
    foreach(MyClass* myclass, list)
    {
        qInfo() << myclass;
    }
}
```

---

## 7-5

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/7-5)

