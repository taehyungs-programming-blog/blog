---
title: "(QML) Enhance Stair Chart with List Properties"
permalink: qt/qml/10/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-13 00:00:00 -0000
last_modified_at: 2021-02-13 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - Paint
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/10-BarChartListPropertiesDemo)

![](/file/image/qml-cpp-interfacing-10-1.png)

```xml
import QtQuick 2.12
import QtQuick.Window 2.12
import com.blikoon.charts 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    StairChart{
        title: "Wheat Consumption by 2030 in Mills of Tons"
        bgColor: "lightgray"
        /*
        //This syntax is just a syntactic sugar.
        Stair{
            from: 50
            value: 250
            color: "yellowgreen"
            text: "RW"
        }
        Stair{
            from: 100
            value: 300
            color: "cyan"
            text: "JP"
        }
        Stair{
            from: 150
            value: 122
            color: "red"
            text: "VT"
        }
        */
        stairs: [
            Stair{
                from: 50
                value: 250
                color: "red"
                text: "RW"
            },
            Stair{
                from: 100
                value: 320
                color: "yellow"
                text: "CN"
            },
            Stair{
                from: 150
                value: 133
                color: "lightblue"
                text: "DE"
            },
            Stair{
                from: 200
                value: 202
                color: "steelblue"
                text: "FR"
            },
            Stair{
                from: 250
                value: 250
                color: "skyblue"
                text: "US"
            },
            Stair{
                from: 300
                value: 200
                color: "beige"
                text: "ZA"
            },
            Stair{
                from: 350
                value: 300
                color: "cyan"
                text: "JP"
            },
            Stair{
                from: 400
                value: 122
                color: "red"
                text: "VT"
            }

        ]
    }
}
```

```cpp
#ifndef STAIR_H
#define STAIR_H

#include <QObject>
#include <QColor>

class Stair : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged) //Color of the Stair rect fill
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)  //Value of the Stair e.g 143
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)  //Text of the Stair e.g RW
    Q_PROPERTY(int from READ from WRITE setFrom NOTIFY fromChanged)      //Starting point of the Stair horizontally e.g 50 . All stairs are 50 wide

public:
    explicit Stair(QObject *parent = nullptr);

    QColor color() const;

    int value() const;

    QString text() const;

    int from() const;

    void setColor(QColor color);

    void setValue(int value);

    void setText(QString text);

    void setFrom(int from);

signals:

    void colorChanged(QColor color);

    void valueChanged(int value);

    void textChanged(QString text);

    void fromChanged(int from);



private :

    QColor m_color;
    int m_value;
    QString m_text;
    int m_from;
};

#endif // STAIR_H
```

```cpp
#ifndef STAIRCHART_H
#define STAIRCHART_H

#include <QObject>
#include <QQuickPaintedItem>
#include "stair.h"

class StairChart : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<Stair> stairs READ stairs NOTIFY stairsChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor NOTIFY bgColorChanged)
    Q_CLASSINFO("DefaultProperty", "stairs")


public:
    explicit StairChart(QQuickItem *parent = nullptr);

    void paint(QPainter * painter);

    QQmlListProperty<Stair> stairs();

    QString title() const;

    QColor bgColor() const;

signals:

    void stairsChanged();

    void titleChanged(QString title);

    void bgColorChanged(QColor bgColor);

public slots:

void setTitle(QString title);

void setBgColor(QColor bgColor);

private :
    static void appendStair(QQmlListProperty<Stair>* list, Stair * stair);
    void drawStair(QPainter *painter, QRect mRect, QColor mColor, qreal barValue,int startingPoint);
    void drawScale(QPainter *painter, QRect mRect);
    QList<Stair*> m_stairs;
    QString m_title;
    QColor m_bgColor;
};

#endif // STAIRCHART_H
```