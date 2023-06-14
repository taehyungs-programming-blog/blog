---
title: "(QML) Editable Object List Model From C++(QPROPERTY)"
permalink: qt/qml/4/                # link 직접 지정
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
  - QPROPERTY
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/4-EditableObjectListModelQpropertyDemo)

기존 코드에는 이름, 나이값을 수정하면 모든 데이터가 리로드 되었다

그 문제를 해결해 보자

---

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
    Q_PROPERTY(QList<QObject*> mypersons READ mypersons WRITE setMypersons NOTIFY mypersonsChanged)
public:
    explicit ObjectListWrapper(QObject *parent = nullptr);
    bool initialize();


    Q_INVOKABLE void setNames(int index, const QString & names);

    Q_INVOKABLE void setAge(int index, const int & age);

    Q_INVOKABLE void setFavoriteColor(int index,const QString & favoriteColor);

    Q_INVOKABLE QList<QObject *> persons() const;

    Q_INVOKABLE void addPerson();

    QList<QObject*> mypersons() const;

    void setMypersons(QList<QObject*> mypersons);

signals:

    void mypersonsChanged(QList<QObject*> mypersons);



private:
    void populateModelWithData();

    void resetModel();

    void printPersons();

    QList<QObject*> mPersons;//This should be QObject*, Person* is not going to work in QML
    QQmlApplicationEngine mEngine;
    //QList<QObject*> m_mypersons;
};

#endif // OBJECTLISTWRAPPER_H

```

```cpp
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
        model : Wrapper.mypersons
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
                    //text : names
                    text : modelData.names
                    Layout.fillWidth: true
                    onEditingFinished: {
                        Wrapper.setNames(index,text)
                    }
                }
                SpinBox{
                    editable: true
                    //value : age //
                    value : modelData.age
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
                    color: modelData.favoriteColor // This format only works for model defined in qml like our mModelId here. // color : model.modelData.favoriteColor// Can also do modelData.favoriteColor directly but adding model makes it clear where the data is coming from. More readable
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