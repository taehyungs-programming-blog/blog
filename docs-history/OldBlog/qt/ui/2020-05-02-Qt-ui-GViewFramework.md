---
title: "Qt : Graphics View Framework 정리"
permalink: qt/ui/GViewFramework/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-05-02 00:00:00 -0000
last_modified_at: 2020-05-02 00:00:00 -0000
sidebar:
  title: "qt"
  nav: qt
---

## 이론

* QGrphicsViw라는 그림판에 
* QGraphicsScene에 저장된 QGraphicsItem을 전시한다고 생각하면 된다.

![](/file/image/qt-gdi-s7-79-image-1.png)

* [79. GraphicsView 핵심 컴포넌트](https://8bitscoding.github.io/Qt-GDI-S7-79/)

---

## 구현

* [80. GraphicsView 활용 mouse 클릭 받으면 네모 출력 위젯 만들기](https://8bitscoding.github.io/Qt-GDI-S7-80/)

더 상세한 구현편은 직접만들면서 보는게 나을 듯... 따로 정리는 안한다.

```cpp
#include "widget.h"
#include "ui_widget.h"
#include "rect.h"
#include "view.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    QGraphicsScene * scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QBrush(QColor(Qt::yellow)));
    scene->setSceneRect(-300,-300,600,600);

    QPen mPen;
    mPen.setWidth(5);
    mPen.setColor(Qt::red);

    Rect * rectItem = new Rect();
    rectItem->setRect(10,10,200,200);
    rectItem->setFlag(QGraphicsItem::ItemIsFocusable);
    rectItem->setFocus();

    rectItem->setPen(mPen);
    rectItem->setBrush(QBrush(Qt::green));

    scene->addItem(rectItem);

    /*
    QGraphicsRectItem * rectItem  = scene->addRect(10,10,200,200);
    rectItem->setPen(mPen);
    rectItem->setBrush(QBrush(Qt::green));
    rectItem->setFlag(QGraphicsItem::ItemIsMovable);
    rectItem->setFlag(QGraphicsItem::ItemIsSelectable);
    */

    //Add lines

    //Horizontal
    scene->addLine(-300,0,300,0);
    //Vertical
    scene->addLine(0,-300,0,300);



    View * view = new View(this);

    /*
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    */

    //view->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    view->setScene(scene);
    ui->verticalLayout->addWidget(view);
    // ui->verticalLayout->addWidget(scene); // -> 이런거 불가능
}

Widget::~Widget()
{
    delete ui;
}
```