---
title: "(Qt) 9. QLinkedList"
permalink: qt/core/QLinkedList/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-02 00:00:00 -0000
last_modified_at: 2020-03-23 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

## 사용법

```cpp
// 선언
QLinkedList<class> name;

// 삽입
name.append(i);
```

---

## 전체코드

코드가 짧아서 아래 깃은 없음.

```cpp
  
#include <QCoreApplication>
#include <QLinkedList>
#include <QDebug>

// QLinkedList<T> is one of Qt's generic container classes.
// It stores a list of values and provides iterator-based access as well as constant time insertions and removals.

// If you need a real linked list, with guarantees of constant time insertions in the middle
// of the list and iterators to items rather than indexes, use QLinkedList.


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QLinkedList<int> list;
    for(int i = 0; i < 10; i++) {
        list.append(i);
    }

    list.removeFirst();
    list.removeLast();
    list.removeOne(5);
    if(list.contains(3)) qInfo() << "Contains 3";
    list.clear();

    qInfo() << "Done";


    return a.exec();
}
```