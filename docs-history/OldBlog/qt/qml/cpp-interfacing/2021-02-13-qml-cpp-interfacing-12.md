---
title: "(QML) Rest Api Client"
permalink: qt/qml/12/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-02-13 00:00:00 -0000
last_modified_at: 2021-02-13 00:00:00 -0000
tag:
  - Qt
  - QML
category:
  - RestApi
sidebar:
  - title: ""
  - nav:
classes: wide
excerpt: ""
header:
  teaser: /file/image/qt-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/12-RestClientV1Demo)
* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/12-RestClientV1Demo2)
* [Get Code](https://github.com/EasyCoding-7/qml-cpp-interfacing/tree/master/12-RestApiClientV2Demo)

![](/file/image/qml-cpp-interfacing-12-1.png)

```xml
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Rest Client V1")

    ColumnLayout{
        anchors.fill: parent
        spacing: 0

        ListView{
            id : mListView
            model : myModel
            delegate: Rectangle{

                width : parent.width
                height: textId.implicitHeight+30
                color: "beige"
                border.color: "yellowgreen"
                radius: 5

                Text {
                    width : parent.width
                    height: parent.height
                    id : textId
                    anchors.centerIn: parent
                    text : modelData
                    font.pointSize: 13
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        SpinBox{
            id : spinBoxId
            Layout.fillWidth: true
            value: 10

        }

        Button {
            id : mButton1
            text : "Fetch"
            Layout.fillWidth: true
            onClicked: {
                Wrapper.fetchPosts(spinBoxId.value)
            }

        }
        Button {
            id : mButton2
            text : "RemoveLast"
            Layout.fillWidth: true
            onClicked: {
                Wrapper.removeLast()

            }

        }
    }
}
```

```cpp
#ifndef APPWRAPPER_H
#define APPWRAPPER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQmlApplicationEngine>

class AppWrapper : public QObject
{
    Q_OBJECT
public:
    explicit AppWrapper(QObject *parent = nullptr);

    Q_INVOKABLE void fetchPosts( int number);

    Q_INVOKABLE void removeLast();

    QStringList jokes() const;

    bool initialize();

signals:

public slots:
private slots:
    void dataReadyRead();
    void dataReadFinished();

private:

    void resetModel () ;

    QNetworkAccessManager * mNetManager;
    QNetworkReply * mNetReply;
    QByteArray * mDataBuffer;
    QStringList mJokes;

    QQmlApplicationEngine mEngine;

};

#endif // APPWRAPPER_H
```

```cpp
#include <QNetworkRequest>
#include <QQmlContext>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include "appwrapper.h"

AppWrapper::AppWrapper(QObject *parent) : QObject(parent),
    mNetManager(new QNetworkAccessManager(this)),
    mNetReply(nullptr),
    mDataBuffer(new QByteArray)
{

}

void AppWrapper::fetchPosts(int number)
{
    //Initialize our API data
    const QUrl API_ENDPOINT("http://api.icndb.com/jokes/random/"+QString::number(number));
    QNetworkRequest request;
    request.setUrl(API_ENDPOINT);

    mNetReply = mNetManager->get(request);
    connect(mNetReply,&QIODevice::readyRead,this,&AppWrapper::dataReadyRead);
    connect(mNetReply,&QNetworkReply::finished,this,&AppWrapper::dataReadFinished);

}

void AppWrapper::removeLast()
{
    if ( !mJokes.isEmpty()){
        mJokes.removeLast();
        resetModel();
    }
}

QStringList AppWrapper::jokes() const
{
    return mJokes;
}

bool AppWrapper::initialize()
{

    mEngine.rootContext()->setContextProperty("Wrapper",this);
    resetModel();

    mEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (mEngine.rootObjects().isEmpty()){
        return false;
    }else{
        return true;
    }
}

void AppWrapper::dataReadyRead()
{
    mDataBuffer->append(mNetReply->readAll());
}

void AppWrapper::dataReadFinished()
{
    //Parse the JSON
    if( mNetReply->error()){
        qDebug() << "There was some error : " << mNetReply->errorString();
    }else{
        //
        //Turn the data into a json document
        QJsonDocument doc = QJsonDocument::fromJson(*mDataBuffer);
        //Grab the value array
        QJsonObject data = doc.object();

        QJsonArray array = data["value"].toArray();

        qDebug() << "Number of jokes " << array.size();

        for ( int i = 0; i < array.size() ; i++){

            QJsonObject object = array.at(i).toObject();

            QString joke = object["joke"].toString();

            mJokes.append(joke);
        }

        if(array.size() !=0){
            resetModel();
        }

        //Clear the buffer
        mDataBuffer->clear();
    }
}

void AppWrapper::resetModel()
{
    mEngine.rootContext()->setContextProperty("myModel",QVariant::fromValue(mJokes));

}
```