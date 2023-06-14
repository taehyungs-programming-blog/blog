---
title: "(Qt) FileConsumer"
permalink: qt/mthread-ipc/fileconsumer/                # link 직접 지정
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
  - FileConsumer
excerpt: ""
classes: wide
header:
  teaser: /file/image/nodejs-page-teaser.gif
---

* [Get Code](https://github.com/EasyCoding-7/Qt-MultiThread-IPC-Example/tree/master/FileConsumer)

---

## 전체코드

```cpp
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemWatcher>

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
    void reloadFile();

private:
    void createFileIfNotExist();
    Ui::Widget *ui;
    QFileSystemWatcher *m_watcher;
};

#endif // WIDGET_H
```

```cpp
#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QDir>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_watcher(new QFileSystemWatcher(this))

{
    ui->setupUi(this);
    setWindowTitle("Consumer Process | Files");
    createFileIfNotExist();
    m_watcher->addPath(QString(QDir::tempPath() + "/sharedFile"));

    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &Widget::reloadFile);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::reloadFile()
{

    QFile inFile(QString(QDir::tempPath() + "/sharedFile"));
    QString transferedStr;
    if (!inFile.open(QFile::ReadOnly)){
        qDebug() << "Consumer , can't find file :" << inFile.fileName();
        return;

    }

    qDebug() <<"Found file : " << inFile.fileName();
    QDataStream in(&inFile);

    ui->textEdit->clear();

    while(!in.atEnd()){
           in >> transferedStr;
           ui->textEdit->append(transferedStr);
       }

    qDebug() << "Consumer done appending data";
}

void Widget::createFileIfNotExist()
{
    QFile file(QString(QDir::tempPath() + "/sharedFile"));
    if(!file.exists()){
        //Just opening the file will create it if it doesn't exist yet.
        file.open(QFile::WriteOnly| QIODevice::Append);
    }
}
```