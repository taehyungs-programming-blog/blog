---
layout: default
title: "Qml 필요할까?"
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

* [참고사이트 🌎](https://8bitscoding.github.io/qt/)

👩‍💻 Qt UI를 구성하는 방법은 크게 두 가지 방법이 있다.<br>
👩‍💻 **.ui** : Qt Creator로 편집이 가능하며 XML구조를 하고 있음.<br>
👩‍💻 **.qml** : Qt Creator로 편집이 역시 가능하며 QML문법을 하고 있음.(JavaScrip와 유사함.)

👩‍💻 우선 간단히 QML 예제를 하나 보자.

<p align="center">
  <img src="https://taehyungs-programming-blog.github.io/blog/assets/images/cpp/qt/qml-1.png"/>
</p>

```cpp
#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    // String List Model을 QML로 넘기고
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

        // 넘겨받은 Model을 Delegate한다
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

👩‍💻 아주 간단한 코드이고 `.ui`를 써본 사람이라면 `.ui`로도 거의 동일하게 간단히 구현할 수 있다<br>
👩‍💻 그럼 qml은 필요가 없는가?<br>
👩‍💻 꼭 그렇지는 않다. qml도 장점이 있는데<br>
👩‍💻 1. UI를 명확하게 qml파일로 **분리** 할 수 있다.<br>
👩‍💻 2. **JavaScript** 를 사용할 수 있다.(아래서 보자)

```xml
// main.qml

import QtQuick 2.12
import QtQuick.Window 2.12
import "utilities1.js" as Utilities1

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Javascript Import Demo")

    Rectangle {
        width : 300
        height: 100
        color: "yellowgreen"
        anchors.centerIn: parent

        Text {
            text : "Click Me"
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
               //console.log("Hello there")
               // Utilities1.greeting()

              //  console.log("Our ages combined yield :" + Utilities1.add(33,17))
                console.log("The difference of 50 and 33 is :"+Utilities1.substract(50,33))
            }

        }
    }
}
```

```xml
// utilities1.js

.import "utilities2.js" as Utilities2
Qt.include("utilities3.js")

function greeting()
{
    console.log("Hello there from external JS file")
}

function combineAges( age1, age2)
{
    console.log("Indirectly calling add() from utilities2")
    return Utilities2.add(age1,age2)
}
```

👩‍💻 대략 qml을 쓰는 이유를 정리해 보자면 ... <br>
👩‍💻 아마 Qt에서 의도한 바는 가벼운 **Front-End는 QML/JavaScript**(`QtQuick`)를 무거운 **Back-End는 C++/Python**으로 작업하라는 뜻으로 보인다.<br>
👩‍💻 **나의 반론** : 보통 Qt개발자는 Native(Win, Mac, Linux) 개발자로 구성이 될 텐데 Font-End/Back-End를 그리 극명하게 나누지 않는다(보통은 둘 다 한다.) 그럼 C++은 기존 Native개발자가 개발을 해왔던 부분이고 새로운 QML이나 JavaScript를 공부해야한다.<br>
👩‍💻 공부를 하는게 싫다는 것이 아니라 효율적이냐에 대한문제이다. Native를 쓰는 이유 중 하나가 속도의 문제인데 과연 QML/JavaScript가 C++보다 빠를까? 구현에 따라 다르겠지만 대부분의 경우 아니라고 본다. 이런 이유에서 QML은 과연 좋은 선택지 일까? 에 대한 의문이 든다.