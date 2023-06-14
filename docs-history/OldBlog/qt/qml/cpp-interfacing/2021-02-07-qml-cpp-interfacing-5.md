---
title: "(QML) Abstract Item Model From C++"
permalink: qt/qml/5/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-07 00:00:00 -0000
last_modified_at: 2021-02-07 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - Item
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/5-AbstractListModelDemo)

![](/file/image/qml-cpp-interfacing-5-1.png)

![](/file/image/qml-cpp-interfacing-5-2.png)

Main

```cpp
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "personmodel.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    PersonModel mModel;

    QQmlApplicationEngine engine;

     engine.rootContext()->setContextProperty("myModel",&mModel);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

```cpp
#ifndef PERSONMODEL_H
#define PERSONMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "person.h"

class PersonModel : public QAbstractListModel
{
    Q_OBJECT
    enum PersonRoles{
        NamesRole = Qt::UserRole + 1,
        FavoriteColorRole,
        AgeRole
    };
public:
    explicit PersonModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex& index) const;

    QHash<int, QByteArray> roleNames() const;

    void addPerson( Person *person);
    Q_INVOKABLE void addPerson();
    Q_INVOKABLE void addPerson(const QString & names,const int & age);
    Q_INVOKABLE void removePerson(int index);
    Q_INVOKABLE void removeLastPerson();

signals:

public slots:
private :
    QList<Person*> mPersons;
};

#endif // PERSONMODEL_H

```

```cpp

#include <QDebug>
#include "personmodel.h"

PersonModel::PersonModel(QObject *parent) : QAbstractListModel(parent)
{
    addPerson(new Person("Jamie Lannister","red",33));
    addPerson(new Person("Marry Lane","cyan",26));
    addPerson(new Person("Steve Moors","yellow",44));
    addPerson(new Person("Victor Trunk","dodgerblue",30));
    addPerson(new Person("Ariel Geeny","blue",33));
    addPerson(new Person("Knut Vikran","lightblue",26));

}

int PersonModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mPersons.size();
}

QVariant PersonModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mPersons.count())
        return QVariant();
    //The index is valid
    Person * person = mPersons[index.row()];
    if( role == NamesRole)
        return person->names();
    if( role == FavoriteColorRole)
        return person->favoriteColor();
    if( role == AgeRole)
        return person->age();
     return QVariant();
}

bool PersonModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Person * person = mPersons[index.row()];
    bool somethingChanged = false;

    switch (role) {
    case NamesRole:
    {
        if( person->names()!= value.toString()){
            qDebug() << "Changing names for " << person->names();
            person->setNames(value.toString());
            somethingChanged = true;
        }
    }
        break;
    case FavoriteColorRole:
    {
        if( person->favoriteColor()!= value.toString()){
            qDebug() << "Changing color for " << person->names();
            person->setFavoriteColor(value.toString());
            somethingChanged = true;
        }
    }
        break;
    case AgeRole:
    {
        if( person->age()!= value.toInt()){
            qDebug() << "Changing age for " << person->names();
            person->setAge(value.toInt());
            somethingChanged = true;
        }
    }

    }

    if( somethingChanged){
        emit dataChanged(index,index,QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags PersonModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> PersonModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NamesRole] = "names";
    roles[FavoriteColorRole] = "favoriteColor";
    roles[AgeRole] = "age";
    return roles;
}

void PersonModel::addPerson(Person *person)
{
    const int index = mPersons.size();
    beginInsertRows(QModelIndex(),index,index);
    mPersons.append(person);
    endInsertRows();
}

void PersonModel::addPerson()
{
    Person *person = new Person("Added Person","yellowgreen",45,this);
    addPerson(person);
}

void PersonModel::addPerson(const QString &names, const int &age)
{
    Person *person=new Person(names,"yellowgreen",age);
    addPerson(person);
}

void PersonModel::removePerson(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    mPersons.removeAt(index);
    endRemoveRows();
}

void PersonModel::removeLastPerson()
{
    removePerson(mPersons.size()-1);
}

```

```xml
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

Window {
    visible: true
    width: 400
    height: 600
    minimumWidth: 400
    minimumHeight: 600
    title: qsTr("Hello World")


    ColumnLayout{
        anchors.fill: parent
        ListView{
            id : mListView
            Layout.fillWidth: true
            Layout.fillHeight: true

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
                        Layout.fillWidth: true
                        onEditingFinished: {
                            console.log("Editing finished, new text is :"+ text + " at index :" + index)
                            model.names = text //The roles here are defined in model class

                        }
                    }

                    SpinBox{
                        id : mSpinbox
                        editable: true
                        Layout.fillWidth: true
                        onValueChanged: {
                            model.age = value;
                        }
                        Component.onCompleted: {
                            mSpinbox.value = model.age
                        }
                    }
                    Rectangle{
                        width : 50
                        height: 50
                        color: model.favoriteColor
                        MouseArea{
                            anchors.fill: parent
                            ColorDialog{
                                id: colorDialog
                                title: "Please choose a color"
                                onAccepted: {
                                    console.log("You chose: " + colorDialog.color)
                                    model.favoriteColor = color
                                }
                                onRejected: {
                                    console.log("Canceled")

                                }
                            }

                            onClicked: {
                                colorDialog.open()

                            }
                        }
                    }
                }
            }
        }

        RowLayout{
            width : parent.width

            Button{
                Layout.fillWidth: true
                height: 50
                text : "Add Person";
                onClicked: {
                    input.openDialog()
                }
                InputDialog{
                    id : input
                    onInputDialogAccepted: {
                        console.log("Here in main, dialog accepted");
                        console.log( " names : " + personNames + " age :" + personAge)
                        myModel.addPerson(personNames,personAge)
                    }
                }


            }
            Button{
                Layout.fillWidth: true
                height: 50
                text : "Remove Last";
                onClicked: {
                    myModel.removeLastPerson()
                }
            }
        }
    }

}

```

```xml
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Item {
    id : rootId
    function openDialog(){
        inputDialog.open()
    }

    property alias personNames : addPerNamesField.text
    property alias personAge: addPerAgeField.value
    signal inputDialogAccepted

    Dialog {
        id: inputDialog

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        width: parent.width/2 //Breaks the binding loop introduced in Qt 5.12.
        parent: Overlay.overlay

        focus: true
        modal: true
        title: "Person data"
        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout {
            spacing: 20
            anchors.fill: parent
            Label {
                elide: Label.ElideRight
                text: "Please enter the data:"
                Layout.fillWidth: true
            }
            TextField {
                id : addPerNamesField
                focus: true
                placeholderText: "Names"
                Layout.fillWidth: true
            }
            SpinBox {
                editable: true
                id : addPerAgeField
                value : 13
                Layout.fillWidth: true
            }

        }

        onAccepted: {
            console.log("Accepted adding person")
            rootId.inputDialogAccepted()

        }
        onRejected: {
            console.log("Rejected adding person")
        }
    }
}
```