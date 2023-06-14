---
title: "(QML) Custom FootBalleam Project"
permalink: qt/qml/14/                # link 직접 지정
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

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/14-ObjectListAndPropertiesDemo)

![](/file/image/qml-cpp-interfacing-14-1.png)

```json
import QtQuick 2.12
import QtQuick.Window 2.12
import com.blikoon.Football 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    FootBallTeam {
        id : team1
        title: "Rayon Sports"
        coatch: "Coatch Name"
        captain: Player{
            name: "Captain"
            position: "Middle Field"
            playing: true
        }

        players: [

            Player{
                name: "Player1"
                position: "Middle Field"
                playing: true
            },
            Player{
                name: "Player2"
                position: "Middle Field"
                playing: true
            },
            Player{
                name: "Player3"
                position: "Middle Field"
                playing: true
            },
            Player{
                name : "Daniel"
                position: "None"
                playing: false
            }



        ]
    }

    ListView {
        anchors.fill: parent
        model : team1.players
        delegate: Rectangle{
            width: parent.width
            height: 50
            border.width: 1
            border.color: "yellowgreen"
            color: "beige"

            Text {
                anchors.centerIn: parent
                text : name
                font.pointSize: 20
            }
        }
    }

    Component.onCompleted: {
        console.log("We have :" + team1.players.length + " players in the team "+ team1.title)
    }
}
```

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "player.h"
#include "footballteam.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    //Register Types
    qmlRegisterType<Player>("com.blikoon.Football", 1,0, "Player");
    qmlRegisterType<FootBallTeam>("com.blikoon.Football", 1,0, "FootBallTeam");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
```
