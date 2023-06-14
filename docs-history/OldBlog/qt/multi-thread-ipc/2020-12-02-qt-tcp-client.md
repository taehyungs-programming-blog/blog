---
title: "(Qt) TcpClient"
permalink: qt/mthread-ipc/tcpclient/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-12-02 00:00:00 -0000
last_modified_at: 2020-12-02 00:00:00 -0000
sidebar:
  title: "Qt 목차"
  nav: Qt
tag:
  - Qt
  - Multi Thread
category:
  - tcpclient
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/TcpClient)

---

## 전체코드

widget.h

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots :
    void socketReady();
    void stateChanged(QAbstractSocket::SocketState socketState);

    void on_lineEdit_textChanged(const QString &newText);

private:
    Ui::Widget *ui;
    QTcpSocket *m_socket;
    bool m_socketReady;
};

#endif // WIDGET_H
```

widget.cpp

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QDataStream>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_socket(new QTcpSocket(this)),
    m_socketReady(false)

{
    ui->setupUi(this);

    setWindowTitle("Client | Data Sender");

    m_socket->connectToHost("localhost", 4040);

    connect(m_socket, &QTcpSocket::connected, this, &Widget::socketReady);
    connect(m_socket,&QTcpSocket::stateChanged,this,&Widget::stateChanged);

}

Widget::~Widget()
{
    m_socket->close();
    delete ui;
}

void Widget::socketReady()
{
    m_socketReady = true;
}

void Widget::stateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << socketState;
}

void Widget::on_lineEdit_textChanged(const QString &newText)
{
    if(m_socketReady){
        QDataStream out(m_socket);
        out << newText;
    }
}

```