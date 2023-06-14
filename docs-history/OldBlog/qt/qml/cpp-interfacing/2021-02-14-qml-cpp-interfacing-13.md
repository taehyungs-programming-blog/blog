---
title: "(QML) Loading QML Objects from C++"
permalink: qt/qml/13/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-14 00:00:00 -0000
last_modified_at: 2021-02-14 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - 
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/13-LoadingQMLFilesConsoleCppDemo)

```json
import QtQuick 2.12

Item {
    objectName: "My Root Item";
    Item{
        id : child1
        objectName: "Child1"
        property int age: 13
        property string names: "Steve Barker"
    }
    Rectangle{
        id : chilRect
        objectName: "childRect"
        width: 200
        height: 300
        color: "green"
    }
    Item{
        id : child2
        objectName: "Child2"
        property string car: "Peugot"
    }
    Item{
        id : child3
        objectName: "Child2"
        property string car: "Ford"

    }
    Item{
        id : child4
        objectName: "deepChild"
        property string level : "level1";
        Item{
            objectName: "deepChild"
            property string level: "level2"
            Item{
                objectName: "deepChild"
                property string level: "level3"
            }
        }
    }
    Item{
        id : child5
        objectName: "deepChild"
        property string level : "level1clone";
    }
}
```

```cpp
#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QColor>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "Running qml app";
    QQmlEngine engine;
    QQmlComponent component(&engine,"qrc:/main.qml");

    QObject * rootObject = component.create();

    qDebug() << "Object name is : " << rootObject->objectName();
    //Print the number of children in the qml file
    qDebug() << "Children count : " << rootObject->children().count();

    //Read Child1

    QObject * child1 = rootObject->findChild<QObject * > ("Child1");
    if( child1){
        qDebug() << "Age is : " << QQmlProperty::read(child1,"age").toInt();
        qDebug() << "Age is : " << child1->property("age").toInt();
        qDebug() << "Names : " << child1->property("names").toString();
    }else {
        qDebug() << "Could not get object with object name Child1";
    }

    //Manipulate rect
    QObject * childRect = rootObject->findChild<QObject*>("childRect");

    if( childRect){
        qDebug() << "The  rect width is " << childRect->property("width").toInt();
        qDebug() << "The rect color is :" << childRect->property("color").toString();
        qDebug() << "The  rect height is " << childRect->property("height").toInt();

        //Modif rect properties
        QColor color(Qt::red);

        childRect->setProperty("width",QVariant::fromValue(600));
        QQmlProperty::write(childRect,"height",QVariant::fromValue(600));
        childRect->setProperty("color",color);

        qDebug() << "Printing out new rect properties";
        qDebug() << "The  rect width is " << childRect->property("width").toInt();
        qDebug() << "The rect color is :" << childRect->property("color").toString();
        qDebug() << "The  rect height is " << childRect->property("height").toInt();
    }

    //Find children

    QList<QObject * > children = rootObject->findChildren<QObject *> ("Child2");

    if( children.count()> 0){
        qDebug() << "Got some children, printing their cars :";
        foreach (QObject * object, children) {
            qDebug() << "The car brand is : " << object->property("car").toString();
        }
    }

    //Recursive and direct children
    QList<QObject * > deepChildren = rootObject->findChildren<QObject *> ("deepChild",Qt::FindDirectChildrenOnly);// Default Recursive
    if ( deepChildren.count( ) > 0){
        qDebug() << "Found " << deepChildren.count()  << " deepChildren, printing out their levels : ";
        foreach (QObject * object, deepChildren) {
            qDebug() << "The child level is : " << object->property("level").toString();
        }
    }

    return app.exec();
}
```

