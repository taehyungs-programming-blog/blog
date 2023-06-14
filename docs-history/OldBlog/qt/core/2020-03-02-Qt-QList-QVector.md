---
title: "(Qt) 5. QList vs QVector"
permalink: qt/core/QList-QVector/                # link 직접 지정
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
QVector<int> vect = QVector<int>();
QList<int> list = QList<int>();

// 삽입
vect.append(data);
list.append(data);

// 선택
vect.at(i);
list.at(i);
```

---

## 전체코드

* [Github](https://github.com/8bitscoding/qtci2-1)

```cpp
#include <QCoreApplication>
#include <QVector>
#include <QList>
#include <QDebug>


template<class T>
void fill(T &container) {
    for(int i = 0; i < 5; i++) {
        container.append(i);
    }
}

template<class T>
void display(T &container) {

    for(int  i = 0; i < container.length(); i++) {
        if(i > 0) {
            auto current = reinterpret_cast<std::uintptr_t>(&container.at(i));
            auto previous = reinterpret_cast<std::uintptr_t>(&container.at(i - 1));
            auto distance = current - previous;

            qInfo() << i  << "At: " << current << " Previous: " << previous << " Distance: " << distance;

        } else {
            qInfo() << i << &container.at(i) << "Distance: 0";
        }
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QVector<int> vect = QVector<int>();
    QList<int> list = QList<int>();

    fill(vect);
    fill(list);

    qInfo() << "Int takes " << sizeof (int) << " bytes of ram";
    qInfo() << "QVector uses continuous locations in memory!";
    display(vect);
    qInfo("");


    qInfo() << "QList uses what ever it can find in memory!";
    display(list);
    qInfo("");

    return a.exec();
}
```