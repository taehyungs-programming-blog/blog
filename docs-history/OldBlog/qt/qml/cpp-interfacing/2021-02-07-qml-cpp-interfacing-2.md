---
title: "(QML) Object List Model From C++"
permalink: qt/qml/2/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-07 00:00:00 -0000
last_modified_at: 2021-02-07 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - Object List Model
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/2-ObjectListModelDemo)

![](/file/image/qml-cpp-interfacing-2-1.png)

## Main

```cpp
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "person.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QList<QObject *> personList;
    personList.append(new Person("John Doe C++","green",32,&engine));
    personList.append(new Person("Mary Green","blue",23,&engine));
    personList.append(new Person("Mitch Nathson","dodgerblue",30,&engine));
    personList.append(new Person("Daniel Sten","red",67,&engine));
    personList.append(new Person("John Doe C++","green",32,&engine));
    personList.append(new Person("Mary Green","blue",23,&engine));
    personList.append(new Person("Mitch Nathson","dodgerblue",30,&engine));
    personList.append(new Person("Daniel Sten","red",67,&engine));
    personList.append(new Person("John Doe C++","green",32,&engine));
    personList.append(new Person("Mary Green","blue",23,&engine));
    personList.append(new Person("Mitch Nathson","dodgerblue",30,&engine));
    personList.append(new Person("Daniel Sten","red",67,&engine));
    personList.append(new Person("John Doe C++","green",32,&engine));
    personList.append(new Person("Mary Green","blue",23,&engine));
    personList.append(new Person("Mitch Nathson","dodgerblue",30,&engine));
    personList.append(new Person("Daniel Sten","red",67,&engine));


    engine.rootContext()->setContextProperty("personModel",QVariant::fromValue(personList));


    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

## Person

```cpp
#ifndef PERSON_H
#define PERSON_H

#include <QObject>

class Person : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString names READ names WRITE setNames NOTIFY namesChanged)
    Q_PROPERTY(QString favoriteColor READ favoriteColor WRITE setFavoriteColor NOTIFY favoriteColorChanged)
    Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)


public:
    explicit Person(QObject *parent = nullptr);
    Person(const QString &names, const QString &favoritecolor,const int &age , QObject * parent = nullptr);

    QString names() const;
    QString favoriteColor() const;
    int age() const;
    void setNames(QString names);
    void setFavoriteColor(QString favoriteColor);
    void setAge(int age);

signals:

    void namesChanged(QString names);

    void favoriteColorChanged(QString favoriteColor);

    void ageChanged(int age);


private:
    QString m_names;

    QString m_favoriteColor;

    int m_age;
};

#endif // PERSON_H
```

```cpp
#include "person.h"

Person::Person(QObject *parent) : QObject(parent)
{

}

Person::Person(const QString &names, const QString &favoritecolor, const int &age, QObject *parent):
    QObject(parent),m_names(names),m_favoriteColor(favoritecolor),m_age(age)
{

}

QString Person::names() const
{
    return m_names;
}

QString Person::favoriteColor() const
{
    return m_favoriteColor;
}

int Person::age() const
{
    return m_age;
}

void Person::setNames(QString names)
{
    if (m_names == names)
        return;

    m_names = names;
    emit namesChanged(m_names);
}

void Person::setFavoriteColor(QString favoriteColor)
{
    if (m_favoriteColor == favoriteColor)
        return;

    m_favoriteColor = favoriteColor;
    emit favoriteColorChanged(m_favoriteColor);
}

void Person::setAge(int age)
{
    if (m_age == age)
        return;

    m_age = age;
    emit ageChanged(m_age);
}
```

```xml
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Object List Model Demo")


    ListView{
        id : mListView
        anchors.fill: parent
        model : personModel
        // 이걸만약 qml로만 쓰고 싶다면?
        // model : mModelId
        delegate: Rectangle {
            height: 90
            radius: 10
            color : "gray"// Can also do modelData.favoriteColor directly but adding model makes it clear where the data is coming from. More readable
            border.color: "cyan"
            width: parent.width
            RowLayout{
                anchors.fill: parent
                anchors.margins: 20
                TextField{
                    text : names
                    // names : qproperty로 연결된 부분임
                    Layout.fillWidth: true
                }
                SpinBox{
                    editable: true
                    value : age 
                    //value : qproperty로 연결
                    Layout.fillWidth: true
                }
                Rectangle{
                    width : 50
                    height: 50
                    color: favoriteColor // This format only works for model defined in qml like our mModelId here. // color : model.modelData.favoriteColor// Can also do modelData.favoriteColor directly but adding model makes it clear where the data is coming from. More readable
                }
            }
        }
    }

    ListModel{
        id : mModelId
        ListElement {
            names : "Daniel Sten"; favoriteColor : "blue"; age : 10
        }
        ListElement {
            names : "Stevie Wongers"; favoriteColor : "cyan"; age : 23
        }
        ListElement {
            names : "Nicholai Ven"; favoriteColor : "red"; age : 33
        }
        ListElement {
            names : "William Glen"; favoriteColor : "yellowgreen"; age : 45
        }
    }
}
```