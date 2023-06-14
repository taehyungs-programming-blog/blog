---
title: "(Qt) property system"
permalink: qt/desing-patterns/property-system/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-25 00:00:00 -0000
last_modified_at: 2020-12-25 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Desing-Pattern
category:
  - property system
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

## Qt property system

* [GetCode](https://github.com/EasyCoding-7/qt-desing-patterns/tree/master/2-10)

qt property system은 qml 환경에서 유용하다.<br>
qml을 통한 example이 되겠다.

* [qproperty system 참고사이트](https://doc.qt.io/qt-5/properties.html)

```cpp
// machine.h
#ifndef MACHINE_H
#define MACHINE_H

#include <QObject>
#include <QTimer>
#include <QDebug>

class Machine : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged) // getter, setter, signal
    Q_PROPERTY(int max READ max WRITE setMax NOTIFY maxChanged) // getter, setter, signal
    Q_PROPERTY(QString name READ name WRITE setName) // getter, setter
public:
    explicit Machine(QObject *parent = nullptr);

    int count() const;
    void setCount(int count);

    int max() const;


    QString name() const;
    void setName(const QString &name);

signals:
    void started();
    void stopped();
    void paused();
    void maxChanged(int value);
    void countChanged(int value);

public slots:
    void start();
    void stop();
    void pause();
    void setMax(int max);

private slots:
    void timeout();

private:
    int m_count;
    int m_max;
    QString m_name;
    QTimer m_timer;

};

#endif // MACHINE_H
```

```cpp
// machine.cpp
#include "machine.h"

Machine::Machine(QObject *parent) : QObject(parent)
{
    m_count = 0;
    m_max = 100;
    connect(&m_timer, &QTimer::timeout,this, &Machine::timeout);
    m_timer.setInterval(1000);
}

int Machine::count() const
{
    return m_count;
}

void Machine::setCount(int count)
{
    m_count = count;
    emit countChanged(m_count);
}

int Machine::max() const
{
    return m_max;
}

void Machine::setMax(int max)
{
    qInfo() << "Set Max :)";
    m_max = max;
    emit maxChanged(m_max);
}

QString Machine::name() const
{
    return m_name;
}

void Machine::setName(const QString &name)
{
    m_name = name;
}

void Machine::start()
{
    qInfo() << this << "Starting";
    m_timer.start();
    emit started();
}

void Machine::stop()
{
    qInfo() << this << "Stopping";
    m_timer.stop();
    emit stopped();
    setCount(0);
}

void Machine::pause()
{
    qInfo() << this << "Pausing";
    m_timer.stop();
    emit paused();
}

void Machine::timeout()
{
    qInfo()  << this << "Counting...";
    m_count++;
    emit countChanged(m_count);

    if(m_count >= m_max) stop();
}
```

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>

/*

  What:
  Q_Property

  Description:
  Qt property system

  Why:
  Easy encapulation and uses the Qt property system

  Example:
  QML App, show how to add the c++ class, and how to access the property in both QML and C++


 */

#include <QDebug>
#include "machine.h"

//Properties in C++
void test()
{
    qInfo() << "Starting C++ test";

    Machine machine;
    machine.setMax(5);

    qInfo() << "Max = " << machine.max();
    machine.setProperty("name","Bryan");
    qInfo() << "Name = " << machine.name();
}


#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    //C++ test
    //test();

    // qml을 등록한다.
    qmlRegisterType<Machine>("com.company.machine",1,0,"Machine");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
```

```xml
import QtQuick.Window 2.12
import QtQuick 2.12
import QtQuick.Controls 2.3
import com.company.machine 1.0

Window {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Machine {
        id: machine

        // @disable-check M16
        onStarted : {
            lblStatus.text = "Started: " + machine.count + " of " + machine.max
        }
        onPaused: {
            lblStatus.text = "Paused: " + machine.count + " of " + machine.max
        }
        onStopped: {
            lblStatus.text = "Stopped: " + machine.count + " of " + machine.max
        }

        onCountChanged: {
            lblStatus.text = "Running: " + machine.count + " of " + machine.max
        }
    }

    Column {
        id: column
        width: 358
        height: 216
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 25

        Label {
            id: lblStatus
            text: qsTr("Status")
            font.pointSize: 25
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            id: row
            width: 350
            height: 75
            spacing: 25

            Button {
                id: btnStart
                text: qsTr("Start")
                onClicked: {
                    console.log("Start the machine")
                    machine.start()
                }
            }

            Button {
                id: btnPause
                text: qsTr("Pause")
                onClicked: {
                    console.log("Pause the machine")
                    machine.pause()
                }
            }

            Button {
                id: btnStop
                text: qsTr("Stop")
                onClicked: {
                    console.log("Stop the machine")
                    machine.stop()
                }
            }
        }

        Button {
            id: btnTest
            text: qsTr("Test")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                console.log("Clicked test")
                machine.setMax(100)
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.8999999761581421}
}
##^##*/
```