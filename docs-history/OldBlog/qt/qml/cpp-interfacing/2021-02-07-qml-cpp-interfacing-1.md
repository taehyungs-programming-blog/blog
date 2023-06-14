---
title: "(QML) String List Model From C++"
permalink: qt/qml/1/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-07 00:00:00 -0000
last_modified_at: 2021-02-07 00:00:00 -0000
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

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/1-StringListModelDemo)

![](/file/image/qml-cpp-interfacing-1-1.png)

## QStringList를 하나의 Model로 사용이 가능

```cpp
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QStringList continentList;
    continentList.append("Africa");
    continentList.append("Europe");
    continentList.append("America");
    continentList.append("Asia");
    continentList.append("Oceania");
    continentList.append("Antarctica");

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("continentModel",continentList);


    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```

```xml
import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("StringList Model Demo")

    ListView{
        id : mListView
        anchors.fill: parent

        model: continentModel
        delegate: Rectangle {
            height: 50
            radius: 10
            color: "dodgerblue"
            border.color: "cyan"
            width: parent.width
            Text {
                text: modelData
                font.pointSize: 20
                anchors.centerIn: parent
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
```

