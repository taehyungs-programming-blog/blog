---
title: "(QML) Decoupling Data Source and Model"
permalink: qt/qml/6/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-13 00:00:00 -0000
last_modified_at: 2021-02-13 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - Decoupling Data
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/6-ExternalDataSource)

![](/file/image/qml-cpp-interfacing-6-1.png)

```cpp
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "datasource.h"
#include "personmodel.h"


int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    PersonModel mModel;
    DataSource * ds = new DataSource(&mModel);
    mModel.setDataSource(ds);

     engine.rootContext()->setContextProperty("myModel",&mModel);
     engine.rootContext()->setContextProperty("myDataSource",ds);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

```cpp
#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>
#include "person.h"

class DataSource : public QObject
{
    Q_OBJECT
public:
    explicit DataSource(QObject *parent = nullptr);

    QList<Person *> dataItems();

    void addPerson( Person *person);
    Q_INVOKABLE void addPerson();
    Q_INVOKABLE void addPerson(const QString & names,const int & age);
    Q_INVOKABLE void removePerson(int index);
    Q_INVOKABLE void removeLastPerson();

signals:
    void prePersonAdded();
    void postPersonAdded();
    void prePersonRemoved(int index);
    void postPersonRemoved();

public slots:
private :
    QList<Person*> mPersons;

};

#endif // DATASOURCE_H
```

```cpp
#include "datasource.h"

DataSource::DataSource(QObject *parent) : QObject(parent)
{
    addPerson(new Person("Jamie Lannister","red",33));
    addPerson(new Person("Marry Lane","cyan",26));
    addPerson(new Person("Steve Moors","yellow",44));
    addPerson(new Person("Victor Trunk","dodgerblue",30));
    addPerson(new Person("Ariel Geeny","blue",33));
    addPerson(new Person("Knut Vikran","lightblue",26));

}

QList<Person *> DataSource::dataItems()
{
    return mPersons;
}

void DataSource::addPerson(Person *person)
{
    emit prePersonAdded();
    mPersons.append(person);
    emit postPersonAdded();
}

void DataSource::addPerson()
{
    Person *person = new Person("Added Person","yellowgreen",45,this);
    addPerson(person);
}

void DataSource::addPerson(const QString &names, const int &age)
{
    Person *person=new Person(names,"yellowgreen",age);
    addPerson(person);
}

void DataSource::removePerson(int index)
{
    emit prePersonRemoved(index);
    mPersons.removeAt(index);
    emit postPersonRemoved();
}

void DataSource::removeLastPerson()
{
    removePerson(mPersons.size()-1);
}
```