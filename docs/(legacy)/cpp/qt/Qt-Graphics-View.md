---
layout: default
title: "Graphics View Framework"
parent: (QT)
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

🔥 결론부터 말하자면, Graphics View Framework을 **왜 쓰냐?**<br>
🔥 Widget에 그려지는 Item 하나하나를 Object로 관리할때 사용된다.

<br>

🔥 (참고) **Scene**아래 **Item**이 들어가며, **View**로 그려진다.<br>
🔥 지금은 무슨 말인가 하겠지만 일단 위의 말을 기억해 두자

---


## Scene, View, Item Example

```cpp
class Rect : public QGraphicsRectItem
{    
public:
    //...
```

```cpp
class View : public QGraphicsView
{
    Q_OBJECT
public:
    // ...
```

```cpp
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Scene을 생성 후 Item 추가
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


    //Horizontal
    scene->addLine(-300,0,300,0);
    //Vertical
    scene->addLine(0,-300,0,300);

    // View에 Scene을 연결
    View * view = new View(this);
    view->setScene(scene);
    // View를 그려주세요
    ui->verticalLayout->addWidget(view);
}
```

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/graphics-view-1.png"/>
</p>

---

## PaintEvent가 있는데??

🔥 Widget에 `PaintEvent`가 있어서 간단히 그릴수 있는데 `Item`이며 `Scene`이며?? 이걸 왜 하지?<br>
🔥 단순 `PaintEvent`를 사용할 경우 Item별 Event처리가 어렵다.<br>
🔥 예를 들어 아래와 같이 네모가 있고 드래그앤 드롭을 받아 이동하려 할때 `PaintEvent`로 처리가 쉬울까 `Item`으로 처리가 쉬울까?

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/graphics-view-2.png"/>
</p>

```cpp
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    Scene * scene = new Scene(this);

    Rect * rectItem = new Rect();
    rectItem->setRect(10,10,200,200);

    scene->addItem(rectItem);

    rectItem->setFlag(QGraphicsItem::ItemIsFocusable);
    rectItem->setFlag(QGraphicsItem::ItemIsMovable);
    rectItem->setFocus();


    View * view = new View(this);
    view->setScene(scene);

    ui->verticalLayout->addWidget(view);
}
```

```cpp
void Rect::keyPressEvent(QKeyEvent *event) {

    qDebug() << "Rect Item : Key press event";
    QGraphicsRectItem::keyPressEvent(event);

}
void Rect::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    qDebug() << "Rect Item : Mouse pressed at : " << event->pos();
    QGraphicsRectItem::mousePressEvent(event);

}
```

---

## 또한 Parent - Child 관계가 형성되어 명령 처리도 수월하다

🔥 아래 그림을 Show/Hide, Delete를 하고싶다면??<br>
🔥 `PaintEvent`로 처리할 수 있으나 모든 변수를 Widget이 보관해야하며 OOP에 어긋난다.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/graphics-view-3.png"/>
</p>

```cpp
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);

    rect1 = new QGraphicsRectItem(-50,-50,100,100);
    rect1->setFlag(QGraphicsItem::ItemIsMovable);
    rect1->setBrush(QBrush(Qt::yellow));


    QGraphicsEllipseItem * ellipse1 = new QGraphicsEllipseItem(-20,-20,40,40);
    ellipse1->setBrush(QBrush(Qt::red));
    ellipse1->setParentItem(rect1);

    QGraphicsEllipseItem * ellipse2 = new QGraphicsEllipseItem(20,20,20,40);
    ellipse2->setBrush(QBrush(Qt::green));
    ellipse2->setParentItem(rect1);


    ImageItem * imageItem = new ImageItem();
    imageItem->setPixmap(QPixmap(":/images/Quick.png"));
    imageItem->setParentItem(rect1);

    scene->addItem(rect1);

    view = new QGraphicsView(this);
    view->setScene(scene);

    ui->verticalLayout->addWidget(view);
}

void Widget::on_showHideButton_clicked()
{
    bool isVisible = rect1->isVisible();
    rect1->setVisible(!isVisible);

}

void Widget::on_removeItem_clicked()
{
    scene->removeItem(rect1);
    delete rect1;

}
```
