---
title: "(QML) Editable Object List Model From C++"
permalink: qt/qml/3/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-07 00:00:00 -0000
last_modified_at: 2021-02-07 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - Editable Object List Model
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/3-EditableObjectListModelDemo)

![](/file/image/qml-cpp-interfacing-3-1.png)

## Q_INVOKABLE

* [참고사이트](https://blog.naver.com/ekthatkxkd/221758877976)

Q_INVOKABLE : QML에서 C++함수를 호출가능

---

## Main

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "objectlistwrapper.h"
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    ObjectListWrapper wrapper;

    if(!wrapper.initialize())
        return -1;

    return app.exec();
}
```

```cpp
#ifndef OBJECTLISTWRAPPER_H
#define OBJECTLISTWRAPPER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "person.h"

class ObjectListWrapper : public QObject
{
    Q_OBJECT
public:
    explicit ObjectListWrapper(QObject *parent = nullptr);
    bool initialize();


    Q_INVOKABLE void setNames(int index, const QString & names);

    Q_INVOKABLE void setAge(int index, const int & age);

    Q_INVOKABLE void setFavoriteColor(int index,const QString & favoriteColor);

    Q_INVOKABLE QList<QObject *> persons() const;

    Q_INVOKABLE void addPerson();

signals:

public slots:

private:
    void populateModelWithData();

    void resetModel();

    void printPersons();

    QList<QObject*> mPersons;//This should be QObject*, Person* is not going to work in QML
    QQmlApplicationEngine mEngine;
};

#endif // OBJECTLISTWRAPPER_H
```

## objectlistwrapper

```cpp
#include <QDebug>
#include "objectlistwrapper.h"

ObjectListWrapper::ObjectListWrapper(QObject *parent) : QObject(parent)
{
    populateModelWithData();
}

bool ObjectListWrapper::initialize()
{

    resetModel();
    mEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (mEngine.rootObjects().isEmpty())
        return false;

    return true;

}

void ObjectListWrapper::setNames(int index, const QString &names)
{
    qDebug() << "Changing names to : " << names;
    if(index >= mPersons.size())
        return;
    Person * person = static_cast<Person *> (mPersons.at(index));
    if( names!= person->names())
    {
        person->setNames(names);
        resetModel();
    }
    printPersons();
}

void ObjectListWrapper::setAge(int index, const int &age)
{
    if(index >= mPersons.size())
        return;
    Person * person = static_cast<Person *> (mPersons.at(index));
    if( age!= person->age())
    {
        person->setAge(age);
        resetModel();
    }
    printPersons();
}

void ObjectListWrapper::setFavoriteColor(int index, const QString &favoriteColor)
{
    if(index >= mPersons.size())
        return;
    Person * person = static_cast<Person *> (mPersons.at(index));
    if( favoriteColor!= person->favoriteColor())
    {
        person->setFavoriteColor(favoriteColor);
        resetModel();
    }
    printPersons();
}

QList<QObject *> ObjectListWrapper::persons() const
{
    return mPersons;
}

void ObjectListWrapper::addPerson()
{
    mPersons.append(new Person("New Person","green",32,this));
    resetModel();
}

void ObjectListWrapper::populateModelWithData()
{
    mPersons.append(new Person("John Doe","green",32,this));
    mPersons.append(new Person("Mary Green","blue",23,this));
    mPersons.append(new Person("Mitch Nathson","dodgerblue",30,this));
}

void ObjectListWrapper::resetModel()
{
    mEngine.rootContext()->setContextProperty("Wrapper",this);
    mEngine.rootContext()->setContextProperty("myModel",QVariant::fromValue(persons()));

}

void ObjectListWrapper::printPersons()
{
    qDebug() << "-------------Persons--------------------";
    foreach (QObject * obj, mPersons) {
        Person * person = static_cast<Person *> (obj);
        qDebug() << person->names() << " " << person->age() << " " << person->favoriteColor();
    }
}
```

## qml

```xml
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    ListView{
        id : mListView
        anchors.fill: parent
        model : myModel
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
                    //text : modelData.names
                    Layout.fillWidth: true
                    onEditingFinished: {
                        Wrapper.setNames(index,text)
                    }
                }
                SpinBox{
                    editable: true
                    value : age //value : modelData.age
                    Layout.fillWidth: true
                    onValueChanged: {
                        if( value === model.age){

                        }else{
                            Wrapper.setAge(index,value)
                        }
                    }
                }
                Rectangle{
                    width : 50
                    height: 50
                    color: favoriteColor // This format only works for model defined in qml like our mModelId here. // color : model.modelData.favoriteColor// Can also do modelData.favoriteColor directly but adding model makes it clear where the data is coming from. More readable
                }
            }
        }
    }

    Button{
        id : mButton
        width: parent.width
        anchors.bottom: parent.bottom
        height: 50
        text : "Add Person"
        onClicked: {
            Wrapper.addPerson()
        }
    }
}

```