---
title: "(Qt) SOLID(Single responsibility principle)"
permalink: qt/desing-patterns/solid/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-24 00:00:00 -0000
last_modified_at: 2020-12-24 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Desing-Pattern
category:
  - SOLID
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [참고사이트](https://ibocon.tistory.com/86?category=813001)

## Single Responsibility

- 정의 : 하나의 클래스는 하나의 기능을 하자 (모든 클래스는 하나의 책임을 부여 받으며, 단 하나의 이유만을 바탕으로 변경되어야 한다.)

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/2-1)

main.cpp

```cpp
#include <QCoreApplication>

/*
 SOLID - https://en.wikipedia.org/wiki/SOLID
 Single Responsibility Principle
 Open/Closed Principle
 Liskov Substitution Principle
 Interface Segregation Principle
 Dependency Inversion Principle

 What
 Single Responsible princible

 Define
 A class should only have a single responsibility, that is, only changes to one part of the software's specification should be able to affect the specification of the class.

 Why
 Makes creating, and maintaining the class easier

 Example
 In this case a good example is a temp converter

 */

#include <QDebug>
#include "tempconverter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TempConverter t;

    int value = 32;
    qInfo() << "c to f" << t.celsiusToFahrenheit(value);
    qInfo() << "f to c" << t.fahrenheitToCelsius(value);

    return a.exec();
}
```

tempconverter.h

```cpp
#ifndef TEMPCONVERTER_H
#define TEMPCONVERTER_H

#include <QObject>

class TempConverter : public QObject
{
    Q_OBJECT
public:
    explicit TempConverter(QObject *parent = nullptr);

    int celsiusToFahrenheit(int value);
    int fahrenheitToCelsius(int value);


signals:

};

#endif // TEMPCONVERTER_H
```

tempconverter.cpp

```cpp
#include "tempconverter.h"

TempConverter::TempConverter(QObject *parent) : QObject(parent)
{

}

int TempConverter::celsiusToFahrenheit(int value)
{
    auto o = (value * 9.0) / 5.0 + 32;
    return o;
}

int TempConverter::fahrenheitToCelsius(int value)
{
    auto o = (value - 32) * 5.0 / 9.0;
    return o;
}
```

---

## Open/Close Principle

- 정의 : 기능 확장에는 열려있어야하고 기능 수정에는 닫혀있어야 한다.

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/2-2)

---

## Liskov Substitution Principle

- 정의 : 자식클래스를 부모클래스 처럼 사용할 수 있어야 한다.

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/2-3)

---

## Interface Segregation Principle

- 정의 : 인터페이스를 구현하는 클래스는 필요한 함수만 구현할 수 있도록 설계

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/2-4)

---

## Dependency Inversion Principle

- 정의 : 상위 모듈과 하위 모듈은 추상적인 약속을 기반으로 소통, 자세한 구현은 추상적 개념에 기반

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/2-5)